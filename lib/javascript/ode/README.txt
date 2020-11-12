2015/7/16
Zerngui Wang

ODE library building
Version: 0.11.1
Platform: Linux

Instruction:
1) Download package and unzip
2) Run command: $emconfigure ./configure --disable-demos --without-x --enable-double-precision
3) Run command: $emmake make
4) Complied libs location:
	./ode/src/joints/.libs/libjoints.a
	./ode/src/.libs/libode.a
	./ode/src/.libs/libfast.a
	./OPCODE/.libs/libOPCODE.a
	./OPCODE/Ice/.libs/libIce.a
Note: we haven't its functionality, we just want to resolve those unresolved symbols during the linking phase.

