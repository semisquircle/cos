import datetime
import math
from pygeomag import GeoMag


class ansi:
	BLUE = "\033[94m"
	GREEN = "\033[32m"
	WHITE = "\033[37m"
	BOLD = "\033[1m"
	ENDC = "\033[0m"


def local_gravity(latitude, height):
	# Constants for gravity model
	g0 = 9.780327  # Gravity at the equator (m/s²)
	k = 0.00193185  # Gravity formula coefficient
	e2 = 0.00669438  # Earth's eccentricity squared
	R = 6378137.0  # Earth's radius in meters

	# Convert latitude to radians
	lat_rad = math.radians(latitude)

	# Normal gravity at sea level
	g_lat = g0 * (1 + k * math.sin(lat_rad) ** 2) / math.sqrt(1 - e2 * math.sin(lat_rad) ** 2)

	# Correct for height
	g_h = g_lat * (1 - (2 * height / R))

	return g_h


def main():
	# Get user input
	print()
	latitude = float(input(f"{ansi.BLUE}Enter latitude (degrees): {ansi.ENDC}"))
	longitude = float(input(f"{ansi.BLUE}Enter longitude (degrees): {ansi.ENDC}"))
	height = float(input(f"{ansi.BLUE}Enter height above sea level (meters): {ansi.ENDC}"))

	# Compute date
	now = datetime.datetime.now()
	year = now.year
	month = now.month
	year_decimal = year + (month / 12)

	# Compute local gravity
	gravity = local_gravity(latitude, height)

	# Compute geomagnetic data
	gm = GeoMag()
	mag = gm.calculate(glat=latitude, glon=longitude, alt=height, time=year_decimal)
	field = mag.ti / 1000
	dec = mag.dec

	# Print results
	print()
	print(f"{ansi.GREEN}Local gravity:{ansi.WHITE}{ansi.BOLD} {gravity:.6f} m/s²{ansi.ENDC}")
	print(f"{ansi.GREEN}Magnetic field:{ansi.WHITE}{ansi.BOLD} {field:.6f} μT{ansi.ENDC}")
	print(f"{ansi.GREEN}Magnetic declination:{ansi.WHITE}{ansi.BOLD} {dec:.6f} deg{ansi.ENDC}")
	print()


if __name__ == "__main__":
	main()
