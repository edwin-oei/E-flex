# E-flex
This project is a proof-of-concept as to how a water desalination / bottle filling plant can operate based on real-time renewable energy input, filtered water level and customer demand. The plant is further powered by battery. We are using an Arduino uno to control the actuators.

There are 3 phases to this project, each of which will be explained further later in this document. The main components of the demonstrator can be seen in the file system_setup.jpg.

There are 2 renewable energy sources: solar and wind. Both have a pre-defined maximum value. To keep the demo simple, there are 3 levels of water demand: none, low and high. There are 3 filtered water levels: low, medium and high.

For the first phase of this project, we shall assume that the renewable energy input and demand are constant. Further, the system operates not using a battery but a constant supply of power. This requires the user to enter the values manually. Specifically, the user is required to input a fraction (in percentage) of both the max solar and wind power input. Should both be 100%, then there should be excess power. The excess power can be used to charge an imaginary battery, the level of which is displayed on an LCD. Should the total input power from renewable sources not suffice to run the system, power will be drawn out from the battery.

In the second phase, the code should be further refined to take real-time renewable energy input into account. In the third phase, the entire system should run also on real-time demand and the use of a real battery shall be implemented. This necessitates the measurement of the battery charge/level.


More about the first phase:

The code for the first phase is almost completed. For the code in the master branch, the conveyor is solely dependent on user demand. No demand means the conveyor stops, low demand and the conveyor moves slowly. High demand and the conveyor moves quickly. Whether the pump is switched on or off is dependent on the water level in tank 2 (where the cleansed water is). In the file called condition_table.xlsx, the relationship between water level, demand, pump and conveyor speed is described.

There are currently 2 other branches for the first phase. The one called nader_request has a different condition compared to the master branch. In the nader_request branch, the conveyour stops when there is no demand or when the water level in tank 2 is low. This is the only difference compared to the master branch. In the third branch, we eschew from the use of local variables, ie we use global variables. This makes the code more readable. The conveyor moves depending on water level and demand, similar to the nader_request branch.
