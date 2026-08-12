// Target: MIG ApplePrivate subsystem seen at 0xfffffff00a960ce0 (SPRR perm-group assign)
// Static: dispatcher switches on msgh_id with values like 0x3000300f, 0x31003c09, 0x320003ff.
// We don't know the hosting port name statically -> scan strategy:
//  (a) bootstrap_look_up over a candidate-name dictionary (edit as learned)
//  (b) raw msgh_id sweep over any port we manage to obtain
// The scan sends EMPTY messages (0x18 header only) and watches for non-MIG_BAD_ID replies.
#include "fuzz.h"
#include <mach/mach.h>

static const char *cand_names[] = {
    "com.apple.sprr", "com.apple.jitbox", "com.apple.pmap", "com.apple.vmapple",
    "com.apple.kernel.sprr", "com.apple.private.sprr", "com.apple.vm.map",
    "com.apple.memory.control", "com.apple.jit", NULL
};

static void sweep(mach_port_t port, const char *tag) {
    for (uint32_t base = 0x30000000; base <= 0x32000000; base += 0x10000) {
        for (uint32_t low = 0; low < 0x10000; low++) {
            uint32_t id = base | low;
            struct { mach_msg_header_t h; } m = {0};
            m.h.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
            m.h.msgh_size = sizeof(m);
            m.h.msgh_remote_port = port;
            m.h.msgh_local_port = mig_get_reply_port();
            m.h.msgh_id = id;
            m.h.msgh_reserved = 0;
            kern_return_t kr = mach_msg(&m.h, MACH_SEND_MSG | MACH_RCV_MSG | MACH_MSG_OPTION_NONE,
                                        sizeof(m), 0x100, mig_get_reply_port(), 10, MACH_PORT_NULL);
            if (kr == 0) {
                // got a reply that wasn't MIG_BAD_ID -> interesting
                uint32_t *r = (uint32_t *)&m;
                if (r[6] != (uint32_t)-301 && r[6] != (uint32_t)-309) {
                    LOG("[mig %s] id 0x%x -> reply ret 0x%x", tag, id, r[6]);
                }
            }
            if ((low & 0x3fff) == 0 && base == 0x30000000)
                LOG("[mig %s] scan at 0x%x", tag, id);
        }
    }
}

void *t_migscan(void *arg) {
    // try candidate bootstrap names
    for (int i = 0; cand_names[i]; i++) {
        mach_port_t p = MACH_PORT_NULL;
        if (bootstrap_look_up(bootstrap_port, cand_names[i], &p) == 0 && p != MACH_PORT_NULL) {
            LOG("[mig] got port for %s: 0x%x", cand_names[i], p);
            sweep(p, cand_names[i]);
        }
    }
    // also sweep host special ports that accept MIG (host_self is not MIG-dispatchable,
    // but the scan is cheap and the reply filter will tell us)
    LOG("[mig] bootstrap-name phase done; add learned names to cand_names[]");
    return NULL;
}
