# Practice
Industrial practice - option 9

# Description
It's project for industrial practice. It's works on ImGui and custom set structure. Task - A set of points on a two-dimensional plane is given. Find a point from which the sum of the distance to other points is the smallest. Implement the "set" data structure as a class. To construct a graphical representation of the problem and its solutions in the form of a set of points. Highlight the found point with a different color.
# Formalization
+ The main technologies that were chosen to implement
the task are OpenGL and ImGui. The main
programming language is C++.
+ OpenGL, rather than Vulkan, was chosen as the interface for interacting with graphical
capabilities due
to the complexity in implementing the second, as well as the lack of support
for some Vulkan technology video cards.
+ To abstract from the low-level work with creating
the window context, the glfw3 library was used.
+ It was decided to implement the program using
a graphical interface (with console support in
the background).
+ The program should have a menu, as well
as a point display area. As well as a table with information about each point.
+ To find a point where the sum of distances to other
points is minimal, we are looking for the total distance of each point to
the others. Then we find the point with the minimum total
distance.
+ As an experiment, the function
of approximate calculation of the point with the smallest distance
through the arithmetic mean point from all points and the distance
between each point and the middle point was implemented. Superior in speed,
but inferior in accuracy, may lead to erroneous
point calculation. In the version for technological practice
, this feature is disabled and an exact algorithm is used.
+ We divide the coordinate system by 50 conventional units.
# Pictures
![изображение](https://user-images.githubusercontent.com/45638806/181905778-cda63860-8185-4aa2-9bf4-69c1d69a536b.png)
