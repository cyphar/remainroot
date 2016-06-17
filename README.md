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

There are two main techniques by which we can shim out these functions.
You should consider your usecase when deciding which one to use.

### `LD_PRELOAD` ###

Any GNU/Linux hacker has certainly misused this feature. Essentially the
`libc` ELF loader (`ld.so`) allows you to specify a set of shared
libraries that will be loaded into the context of the binary. The
symbols provided by that library can be used to override weak symbols
(such as many of the symbols provided by `libc`). This allows us to add
our shims **on the `libc` layer**.

Because `LD_PRELOAD` operates on a `libc` layer, if a process decides to
make us raw syscalls, or uses a library that uses raw syscalls then our
shim won't fire. This obviously can cause problems because then you're
back at square one. However, most programs will act nicely with
`LD_PRELOAD`, and this code is much simpler and less hacky than the
other main mechanism for syscall shims.

It should be noted that because `libc` implements a POSIX API, the shims
used within the `LD_PRELOAD` technique are specifically written to
emulate POSIX semantics. The underlying Linux syscalls **do not** obey
POSIX semantics, so we have to also emulate some of the hacks that
`glibc` uses to fool processes into thinking that things like
`setuid(2)` actually obey POSIX on GNU/Linux.

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

My view on the licensing of the shared library used by `LD_PRELOAD` is
that using a debugging facility that your program does not directly
depend on (doesn't actually dynamically link to) is the "standard use"
of this particular library (that's what it was designed to do after
all). However, **this is not a binding agreement** and **should not be
interpreted to be any additional permissions to the above license**.
`remainroot` is licensed under the **verbatim** license text as
referenced above, with no additional permissions or exceptions. Please
consult with a lawyer or email me for clarification or permission before
deciding to potentially infringe on the license.
