source=sidshield.ino

all:
	arduino --verify $(source)

upload:
	arduino --upload $(source)
