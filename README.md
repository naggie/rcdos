RCDOS: Bluetooth RCD ground-fault device
----------------------------------------

Most RCDs trip at 30mA or so. Therefore, if a relay is used to connect live to
earth via a resistor, the RCD could trip. The device could be controlled over
bluetooth and also verify success. A supercap or battery backup system could be
used.

Some buildings will have independent fast-action RCDs, with an upstream 100mA
time-delayed RCD. It may be possible to trip the upstream RCD.

The resistance must be chosen to ensure a trip without dissipating too much
power. The RMS voltage is 240V, and assuming a 100mA trip current a resistance
of 2K4 can be used. This will result in 24W of power being dissipated which is
a lot. Therefore, the thermal inertia of a power resistor must be used to send
a trip impulse, EG: 0.5 seconds. A duty cycle of 60:1 (arbitrary) could be
observed.

The system could power an ATMega328 from battery or mains, and then accept
serial commands over Bluetooth. For example, with echo on:

	Welcome to RCDOS.
	Mains is online.
	? for help.
	T
	Attempting trip...
	Success! Mains power failure.
	t
	Temperature of ballast is 65c
	?
	T : Attempt mains trip
	t : Report ballast temperature
	? : help
	c : Check mains status
	b : get battery voltage
	s : ACK
	T
	Error: Mains is already offline.
	c
	Mains is online.
	T
	Error: Ballast is too hot

The device may trip the local circuit, or possibly an entire phase/building,
depending on the strength of the ballast. Different values may be compared.


A 5v mains supply could be used with a super capacitor to run through the
blackout. Arduino+ bluetooth module could work down to 2v with the right level
converter. Clever scheduling could allow sleep mode to save power and run for a
long time, refusing to operate the Relay.

It could also measure the time it took for the mains to trip.

Also measure battery voltage.
