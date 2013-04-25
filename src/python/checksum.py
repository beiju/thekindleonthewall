from arduino import Utils
import sys

sys.argv.pop(0)
sys.stdout.write(Utils.checksum(" ".join(sys.argv).strip()))