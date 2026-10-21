// Copyright 2026 The Crashpad Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "util/misc/capture_context.h"

namespace crashpad {

__attribute__((naked)) void CaptureContext(CONTEXT* context) {
    // We must use raw inline assembly to read and store x0-x30, sp, pc, and cpsr into the CONTEXT struct.
    // We pass the pointer context in x0 (standard ARM64 calling convention passes the first argument in x0!).
    // So we can just write the raw assembly statements directly!
    __asm__ volatile(
        // Save general purpose registers x0-x29
        "stp x0, x1, [x0, #0x008]\n"
        "stp x2, x3, [x0, #0x018]\n"
        "stp x4, x5, [x0, #0x028]\n"
        "stp x6, x7, [x0, #0x038]\n"
        "stp x8, x9, [x0, #0x048]\n"
        "stp x10, x11, [x0, #0x058]\n"
        "stp x12, x13, [x0, #0x068]\n"
        "stp x14, x15, [x0, #0x078]\n"
        "stp x16, x17, [x0, #0x088]\n"
        "stp x18, x19, [x0, #0x098]\n"
        "stp x20, x21, [x0, #0x0a8]\n"
        "stp x22, x23, [x0, #0x0b8]\n"
        "stp x24, x25, [x0, #0x0c8]\n"
        "stp x26, x27, [x0, #0x0d8]\n"
        "stp x28, x29, [x0, #0x0e8]\n"
        // Save LR (x30)
        "str x30, [x0, #0x0f8]\n"
        // Save SP
        "mov x1, sp\n"
        "str x1, [x0, #0x100]\n"
        // Save PC (which is the return address held in LR/x30!)
        "str x30, [x0, #0x108]\n"
        // Save CPSR (NZCV bits) at context.Cpsr (offset 0x004)
        "mrs x1, nzcv\n"
        "str w1, [x0, #0x004]\n"
        // Set ControlFlags (CONTEXT_ARM64 | CONTEXT_CONTROL | CONTEXT_INTEGER)
        // context.ContextFlags is at offset 0.
        "mov w1, #0x0003\n"
        "movk w1, #0x0040, lsl #16\n"
        "str w1, [x0]\n"
        // Restore x1 from saved context to avoid corrupting caller state
        "ldr x1, [x0, #0x010]\n"
        "ret\n"
    );
}

}  // namespace crashpad
