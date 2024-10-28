# Project Documentation for CS 380: Convex Hull from Line Intersections

## Problem Overview
This project addresses the problem of managing `n` lines in a two-dimensional plane, where every two lines intersect. The goal is to generate these lines, visualize them, and determine the intersection points. From these intersection points, a convex hull is constructed.

## How It Works
1. **Line Representation**: Each line is represented by the equation \(y = kx + l\), where `k` is the slope and `l` is the y-intercept. The project defines a `Line` structure to store and manage these parameters.

2. **Line Generation**:
   - Randomly generate lines based on user input for `n`.
   - Calculate the coefficients \(k\) and \(l\) for each line.
   - Draw the lines on a canvas, ensuring that the intersection points are within the visible area.

3. **Manual Drawing**: Users can manually draw lines by selecting two points on the canvas. The project ensures that lines are only drawn if their intersection points fall within the canvas boundaries.

4. **Convex Hull Calculation**: After determining the intersection points of the lines, the project includes an algorithm to compute the convex hull from these points. This algorithm operates with a time complexity of `O(nlogn)`, optimizing for efficiency.

## Integration with Class Project
This functionality has been added to the project created by the teaching assistant for the CS 380 class, enhancing the existing project with line management, visualization capabilities, and the construction of convex hulls from intersection points.
