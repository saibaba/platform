previous_handler = trap('TERM') { raise RuntimeError, "signal" }

puts "prev handler:"
puts previous_handler

myhandler = trap('TERM', previous_handler)
puts "temp handler:"
puts myhandler

nhandler = trap('TERM', previous_handler)
puts "handler after restore:"
puts nhandler

puts `echo test self-kill; kill $$; echo should not get here, that is a 
bug`

trap('TERM', "DEFAULT")
defhandler = trap('TERM', "DEFAULT")
puts "handler should be default, is:"
puts defhandler
