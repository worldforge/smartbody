
global receiveMode
global testSpecialCases
global argv
global numMessagesReceived

set NUM_MESSAGES 20000
set numMessagesReceived 0
set timeBefore 0
set timeAfter 0


proc elbench {args} {

	global testSpecialCases

	if { $testSpecialCases == 1 } {
	
		puts "received - 'elbench $args'"
	
	} else {
	
		global NUM_MESSAGES
		global numMessagesReceived
		global timeBefore
		global timeAfter
		
		incr numMessagesReceived 1

		if { $numMessagesReceived % 2000 == 0 } {
			puts "$numMessagesReceived messages received - $args"
		}

		if { $numMessagesReceived > 0 && $timeBefore == 0 } {
			set timeBefore [clock clicks -milliseconds]
		}

		if { $numMessagesReceived >= $NUM_MESSAGES } {
			set timeAfter [clock clicks -milliseconds]

			puts "Time to receive $NUM_MESSAGES messages: [expr $timeAfter - $timeBefore]"

			set numMessagesReceived 0
			set timeBefore 0
		}
	}
}




ttOpen notices


if { [lindex $argv 0] == 1 } {
	set receiveMode 0
} else {
	set receiveMode 1
}

if { [lindex $argv 1] == 1 } {
	set testSpecialCases 1
} else {
	set testSpecialCases 0
}

if { $receiveMode == 1 } {

	puts "Receive Mode"
	
	if { $testSpecialCases == 1 } {
		puts "Testing special case messages"
	}
	
	ttRegister observe elbench
	
} else {

	puts "Send Mode"

	if { $testSpecialCases == 1 } {

		global s

		puts "Testing special case messages"

		set s "Test Message: 'Single-Quote'"
		puts "Sending $s"
		ttNotice elbench $s

		set s "Test Message: \"Double-Quote\""
		puts "Sending $s"
		ttNotice elbench $s

		set s "Test Message: \[Bracket\]"
		puts "Sending $s"
		ttNotice elbench $s

		set s "Test Message: {Brace}"
		puts "Sending $s"
		ttNotice elbench $s

		set s "Test Message: \\Backslash\\"
		puts "Sending $s"
		ttNotice elbench $s

		set s "Test Message: /Slash/"
		puts "Sending $s"
		ttNotice elbench $s

		set s "Test Message: (Parenthesis)"
		puts "Sending $s"
		ttNotice elbench $s

		set s "Test Message: <Angled Parens?>"
		puts "Sending $s"
		ttNotice elbench $s

		set s "Test Message: `Dunno what these are called`"
		puts "Sending $s"
		ttNotice elbench $s

		set s "Test Message: \"~!@#$%^&*()_+\" - top row special characters"
		puts "Sending $s"
		ttNotice elbench $s
		
		after 5000

	} else {
	
		set timeBefore [clock clicks -milliseconds]
		
		for {set i 0} {$i < $NUM_MESSAGES} {incr i 1} {
			ttNotice elbench "$i Test Message"

#			after 100

			if { $i % 2000 == 0 } {
				puts "$i messages sent"
			}
		}

		set timeAfter [clock clicks -milliseconds]

		puts "Time to send $NUM_MESSAGES messages: [expr $timeAfter - $timeBefore]"
	}
	
	exit
}
