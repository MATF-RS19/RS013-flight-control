# Flight Control

## :airplane: About
Application that animates airplane flight control

## :wrench: Setup
`$ git clone https://github.com/MATF-RS19/RS013-flight-control`

`$ qmake`

`$ make`

`$ ./FlightControl`

## :information_source: Usage

Random flights are created automatically between airports. When multiple airplanes are trying to land at the same airport, the airport will create the landing order such that it minimizes the amount of fuel that is spent, by using the local search algorithm. 

Different types of airplanes fly at different heights, and therefore cannot collide. Those who can collide will try to avoid collisions, but in some cases, help from the user is required. The log window will display notifications about flights, as well as display information about individual airplanes.

### Map navigation
User can navigate the map using <kbd>MOUSE WHEEL</kbd>. Scrolling will zoom in or out, and holding the<kbd>MOUSE WHEEL</kbd> and moving the mouse will move the view.

### Airplane controls
User can select an airplane with <kbd>LEFT CLICK</kbd>. Pressing the <kbd>F</kbd> key gives the user full control over the selected airplane. Use <kbd>⇦</kbd> and <kbd>⇨</kbd> to steer and <kbd>⇩</kbd> to land (if an airport is selected the plane will land there). Pressing <kbd>⇧</kbd> switches back to auto-pilot.

### Flight management
User can select up to two airports with <kbd>LEFT CLICK</kbd>. Pressing <kbd>1</kbd> will send an airplane from the first selected airport to the other. User can change which type of airplane is created by pressing <kbd>2</kbd> and <kbd>3</kbd>. Pressing the <kbd>RIGHT CLICK</kbd> while airport building is off will spawn an airplane at the cursor position and send it to selected airport (nearest one if no airport is selected).

### General controls
Pressing the <kbd>B</kbd> key toggles airport building. While it is on, pressing the <kbd>RIGHT CLICK</kbd> will create a new airport at the cursor position. Name for the airport is set to whatever is in the text input box in the log window. Pressing <kbd>D</kbd> deletes all selected items. User can make both types of changes permanent by pressing the <kbd>S</kbd> key or clicking the 'Save' button on the log window.


## :blue_book: Prerequisites
* Qt5
* C++11

##  :computer: Authors
* **Filip Jovanovic**: [github](https://github.com/jovanovic16942)
* **Stefan Kapunac**: [github](https://github.com/StefanKapunac)
* **Marija Milicevic**: [github](https://github.com/marija1023)
