## `remainroot` ##

`remainroot` is a tool that shims out many different functions in order
to trick a process into thinking that it is able to change its
credentials. This should go without saying, but **do not run this code
outside of a rootless container**. **It disables any security that your
program gets from dropping privileges, and lies to the program about its
current privilege level**. This program used to be called "Beware the
Leopard". Apart from being a Douglas Adams reference, I wanted people to
be aware that this code should be used with fear, not in anger.

All of that being said, here's some more background. Inside a rootless
container, certain syscalls will always fail. Some of these failures are
caused by the fact that we can only map a single `uid` and `gid` inside
the user namespace. Others fail because of restrictions by the kernel
for security reasons. Furthermore, some syscalls will return results
that will confuse some processes by providing seemingly garbage results.
`remainroot` is designed to shim out all of these calls so that an
unmodified process can run perfectly fine inside a rootless container.

In the past, `remainroot` gave users a choice between using an `LD_PRELOAD`
library (which would shim out `glibc` library calls and had effectively no
performance impact) and a `ptrace(2)`-style shim (which would work with any
GNU/Linux binary but had a massive performance impact). Due to a very large
number of bugs and design issues in `LD_PRELOAD` (when it comes to
`fork(2)`ing), `remainroot` only supports the `ptrace(2)` shim.

### `ptrace(2)` ###

`ptrace(2)` is a debugging interface inside the Linux kernel, and is
used primarily by tools like `strace(1)` and `gdb(1)`. It is quite a
powerful mechanism, but has so many intricacies that make it quite
complicated and potentially dangerous to use. In addition, it can make
programs quite slow (though `remainroot` attempts to minimise this by
only using `PTRACE_SYSCALL` and a few other things). In addition,
programs that create threads or fully fork are quite complicated to keep
track of.

The reason that you might want to use `ptrace(2)` is because **it
operates on the syscall layer**. This means that all of the potential
problems listed in `LD_PRELOAD` go away, and `ptrace` is the "right way"
of solving the syscall shim problem. However, as I mentioned above,
`ptrace(2)` is incredibly fragile (and has quite a few
architecture-specific things embedded inside it).

Since `ptrace(2)` operates on the syscall layer, we have to emulate the
Linux semantics and leave it to `glibc` to do POSIX emulation with the
result of our shims. While this does mean having two layers of lies
stacked on top of each other, this is a more complete solution than
`LD_PRELOAD`. In addition, it solves some problems with `fork` and
`execve` that are not really possible to entirely solve with
`LD_PRELOAD`.

### License ###

`remainroot` is licensed under the GNU GPLv3 or later.

```
remainroot: a shim to trick code to run in a rootless container
Copyright (C) 2016 Aleksa Sarai <asarai@suse.de>

remainroot is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

remainroot is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with remainroot.  If not, see <http://www.gnu.org/licenses/>.
```
