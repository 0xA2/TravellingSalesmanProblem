from shapely.geometry import Polygon
import matplotlib.pyplot as plt

def plot():
	try:
		with open("tour.txt","r") as f:
			p = []
			Lines = f.readlines()
			for l in Lines:
				l = l[:-1]
				p.append((int(l.split(" ")[0],10),int(l.split(" ")[1],10)))
			polygon = Polygon(p)
			x,y = polygon.exterior.xy
			plt.plot(x,y,'ko')
			plt.plot(x,y,'k-')
			plt.show()

	except IOError:
		print ("File does not exist")

def main():
	plot()
if __name__ == "__main__":
	main()
