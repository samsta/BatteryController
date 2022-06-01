usage:
mitm <can_interface1> <can_interface2> <serial # (decimal)> [<id_to_drop1 (hex)> .. <id_to_dropN (hex)>]


LOGGING FILTER CONFIGURATION 
use the mitm-log-filter.txt file to configure message logging 

format
<canid (hex)> <filter bit mask 8 x 8 bits (hex)>

example 1
6ff 00 00 00 00 00 00 00 01
log the message whenever the first bit changes

example 2
6ff ff 00 00 00 00 00 00 00
log the message whenever the highest byte (8 bits) changes

example 3
6ff ff ff ff ff ff ff ff ff
log the message whenever any part of it changes
does not log messages which are idential to the previous message received

special cases
6ff aa aa aa aa aa aa aa aa
log ALL messages for this canid regardless of message content

000 aa aa aa aa aa aa aa aa
if the canid is 000 in the configuratin file, ALL canids received will be logged
the rest of the configuration file will be ignored


MESSAGE REPLACE CONFIGURATION
use the mitm-msg-replace.txt fileto configure message replacement

format
<canid> <skip #> <send #> <message 8 x 8 bits (hex)>

 skip # =  number of messages to allow to pass unchanged before sending the replacement messages
 if <skip #> is 0, no messages will be skipped

 send # =  number of replacement messages to send, then revert to passing the message unchanged
 if <send #> is 0, replacement message will always be sent
 
 example 1
701 0 0 ca fe fa ce de ad be ef
no messages skipped, all messages send with cafefacedeadbeef

example 2
701 5 10 ca fe fa ce de ad be ef
pass 5 messages unchanged, then send cafefacedeadbeef for 10 messages, then send message unchanged

