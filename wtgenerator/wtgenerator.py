# Wavetable generator, by Michael Mulshine
# Use: python wtgenerator.py NAME DOMAINSIZE SAMPLERATE BASEFREQ
# As configured, will generate a set of wavetables for a square wave 
# starting at a base frequency and jumping up octaves until Nyquist.
#
# python wtgenerator.py SQR 2048 44100 20
#
# Expected output: 
#   SQE_full.txt (all wavetables configured in floating point two dimensional array)
#   SQE_XXXX.txt (Individual wavetables for frequency XXXX.)
#   SQE_XXXX.png (Plots for frequency XXXX, concatenated on previous.)
#

import sys, re, string, numpy, math
import matplotlib.pyplot as plt

def clip(lo, x, hi):
    return max(lo, min(hi, x))

# Phasor
inc = 1.0/float(sys.argv[2])
phase = 0.0 

pi = 3.14159265
two_pi = 2 * pi

# Shaper
sqrt8 = 2.82842712475
wscale = 1.30612244898
m_drive = 0.1

# Feedback
lowf = 0.05 # period 20Hz
highf = 0.0002604165 # period 960*4

# Counters
i = 0
n = 0

period = 0.0002604165
# Main

tablenum = 1
outputfile = open(sys.argv[1] + "_" + sys.argv[4],"w")
outputfile.truncate(0)

outputfile2 = open(sys.argv[1] + "_full","w")
outputfile2.truncate(0)

nyquist = float(sys.argv[3]) / 2.0

def feedback(samp,period):
	return math.pow(0.5, (period/(samp*10)))

def shaper1 (samp):
	fx = ((samp * 4.0) - 2.0)
	xc = clip(-sqrt8, fx, sqrt8)
	xc2 = xc*xc
	c = 0.5*fx*(3.0 - (xc2))
	xc4 = xc2 * xc2
	w = (1.0 - xc2*0.25 + xc4*0.015625) * wscale
	shaperOut = w*(c+ 0.05*xc2)*(m_drive + 0.75)
	return shaperOut

def adc_lookup(samp):
	return math.pow( 0.5, (1.0/(samp * 48000.0)))

def tanh_lookup(samp):
	return math.tanh((samp * 4.0) - 2.0)

def mtof_lookup(samp):
	return (440.0 * math.pow( 2.0, ((samp*109.0+25.0)-69.0)/12.0))

def mtof(note):
	return (440.0 * math.pow( 2.0, (note-69.0)/12.0))

def filtertan(samp):
	return math.tan(3.14159265 * (mtof(samp*114.0+16.0)/48000.0))

def envelope_decay(samp):
	return math.pow(samp, 1.000005)

def envelope_decay2(samp):
	return math.pow((1.0-samp), 2.0)

def inverseAttackDecayIncrements(samp):
	if (samp == 0):
		return 65536.0
	else:
		return 65536.0/((samp * 8.192) * 48000.0)

def inverseRate(samp):
	return 1000.0/(1 + samp * 9999.0)


count = 0
famp = 1.0
base = float(sys.argv[4])
freq = 0
harm_step = 2
freq_scale = pow(2,harm_step)

while (base < nyquist):

	print(str(base))

	outputfile = open(sys.argv[1] + "_" + str(int(base)),"w")
	outputfile.truncate(0)

	plotout = []

	wave = [ 0 ] * int(sys.argv[2])

	harmonic = 1

	freq = base

	while (freq < nyquist):
        
		while (phase < 1.0): 
			this_sine = (famp/harmonic) * math.sin(harmonic * phase * two_pi)
			wave[count] += this_sine

			count += 1
			phase += inc

			i = i+1
			if i >= 20:
				i = 0

		phase = 0.0
		count = 0
		harmonic += harm_step
		freq = harmonic * base


	j = 0

	outputfile2.write("\n{\n")

	while (j < int(sys.argv[2])):

		outputfile2.write(str(round(float(-wave[j]),6)))
		outputfile.write(str(round(float(-wave[j]),6)))

		if (i == 19):
			outputfile2.write("f,\n")
			outputfile.write("f,\n")
		else:
			outputfile2.write("f, ")
			outputfile.write("f, ")

		if (n == 2):
			plotout.append(-wave[j])
			#plt.plot(sampin, shaperOut, lw=10)
			n = 0
		n += 1

		j += 1

	outputfile2.write("\n},\n")

	plt.clf
	plt.plot(plotout)
	plt.ylabel('Output')
	plt.xlabel('Input')
	plt.savefig(sys.argv[1] + "_" + str(int(base)) + ".png")
	tablenum += 1
	outputfile.flush()

	base*=2






