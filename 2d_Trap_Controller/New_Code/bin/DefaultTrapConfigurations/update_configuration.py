import numpy as np
import sys
import socket
import signal

# Exit gracefully when Ctrl-C is entered.
def signal_handler(signal, frame):
    print
    print "Auf Wiedersehen!"
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)



# Which file should we load our current trap configuration from and
# then subsequently update?
# NOTE: This file will be overwritten! Make sure to copy it elsewhere if you
# want to preserve the configuration as is.
filename = raw_input("Enter filename to update: ")


# Load file contents (or quit if file doesn't exist.)
try:
    f = open(filename, "r")
    contents = f.read()
    f.close()
except:
    print "Unable to open file: %s" %filename
    sys.exit(0)


# Parse contents into frequencies, amplitudes, and phases.
# Each line in configuration file looks like: "[freq] [amp] [phase]"
#
# Here we won't be changing frequencies or phases, so we'll leave them
# as strings. The amplitudes, however, will be parsed as floats so that
# we can update them with rescaling factors.
freqs = []
amps = []
phases = []

lines = contents.split("\n")
for line in lines:
    if line.strip() == "":
        continue

    components = line.split(" ")
    freqs.append(components[0])
    amps.append(float(components[1]))
    phases.append(components[2])

if len(freqs) != len(amps) or len(freqs) != len(phases):
    print "%d frequencies, %d amplitudes, %d phases. WTF?" %(len(freqs), len(amps), len(phases))
    sys.exit(0)

print "Read in %d traps from configuration file." %len(freqs)




try:
    # Create socket structure.
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('', 1236))
    s.listen(1)

    while True:
        # Wait to receive a connection that will ask us to rescale these amplitudes.
        print
        print "Waiting to receive connection... (Ctrl-C to exit)"

        # Wait to receive a connection.
        conn, addr = s.accept()
        print "Obtained data from", addr

        # Receive data from connection.
        received_data = ""
        while True:
            packet = conn.recv(1024)
            if not packet:
                break
            received_data += packet
        conn.close()

        # Parse received data to obtain amplitude scale factors.
        # The data should be sent with a single amplitude scale factor on each line,
        # sorted by frequency (we always expect the configuration files to have
        # traps sorted by frequency).
        amplitude_scale_factors = []

        lines = received_data.split("\n")
        for line in lines:
            if line.strip() == "":
                continue

            amplitude_scale_factors.append(float(line))
        
        # We should have exactly one amplitude scale factor for each frequency in our
        # list of traps.
        if len(amplitude_scale_factors) == len(amps):

            # Update amplitudes.
            for index in range(len(amps)):
                amps[index] *= amplitude_scale_factors[index]

            # Write new traps to replace the previous version of our file.
            new_file_contents = ""
            for i in range(len(freqs)):
                new_file_contents += "%s %.6f %s\n" %(freqs[i], amps[i], phases[i])

            # Write new trap configuration to replace the one we started with.
            f = open(filename, "w")
            f.write(new_file_contents)
            f.close()

            print "Updated trap amplitudes!"
        else:
            print "Didn't receive the correct number of amplitude scale factors!"
            print "Received %d factors (expected %d)" %(len(amplitude_scale_factors), len(amps))




except socket.error, msg:
    print "Error:", msg
