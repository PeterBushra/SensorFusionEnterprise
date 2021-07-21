function [allData, scenario, sensor] = generateSensorData()
%generateSensorData - Returns sensor detections
%    allData = generateSensorData returns sensor detections in a structure
%    with time for an internally defined scenario and sensor suite.
%
%    [allData, scenario, sensors] = generateSensorData optionally returns
%    the drivingScenario and detection generator objects.

% Generated by MATLAB(R) 9.4 and Automated Driving System Toolbox 1.2.
% Generated on: 08-Jan-2020 23:56:17

% Create the drivingScenario object and ego car
[scenario, egoCar] = createDrivingScenario;

% Create all the sensors
sensor = createSensor(scenario);

allData = struct('Time', {}, 'ActorPoses', {}, 'ObjectDetections', {}, 'LaneDetections', {});
running = true;
while running
    
    % Generate the target poses of all actors relative to the ego car
    poses = targetPoses(egoCar);
    time  = scenario.SimulationTime;
    
    % Generate detections for the sensor
    laneDetections = [];
    [objectDetections, numObjects, isValidTime] = sensor(poses, time);
    objectDetections = objectDetections(1:numObjects);
    
    % Aggregate all detections into a structure for later use
    if isValidTime
        allData(end + 1) = struct( ...
            'Time',       scenario.SimulationTime, ...
            'ActorPoses', actorPoses(scenario), ...
            'ObjectDetections', {objectDetections}, ...
            'LaneDetections',   {laneDetections}); %#ok<AGROW>
    end
    
    % Advance the scenario one time step and exit the loop if the scenario is complete
    running = advance(scenario);
end

% Restart the driving scenario to return the actors to their initial positions.
restart(scenario);

% Release the sensor object so it can be used again.
release(sensor);

%%%%%%%%%%%%%%%%%%%%
% Helper functions %
%%%%%%%%%%%%%%%%%%%%

% Units used in createSensors and createDrivingScenario
% Distance/Position - meters
% Speed             - meters/second
% Angles            - degrees
% RCS Pattern       - dBsm

function sensor = createSensor(scenario)
% createSensors Returns all sensor objects to generate detections

% Assign into each sensor the physical and radar profiles for all actors
profiles = actorProfiles(scenario);
sensor = radarDetectionGenerator('SensorIndex', 1, ...
    'SensorLocation', [3.7 0], ...
    'Yaw', 0.485545830008134, ...
    'MaxRange', 100, ...
    'ActorProfiles', profiles);

function [scenario, egoCar] = createDrivingScenario
% createDrivingScenario Returns the drivingScenario defined in the Designer

% Construct a drivingScenario object.
scenario = drivingScenario;

% Add all road segments
roadCenters = [14.6 17.8 0;
    42.1 7.8 0;
    46.1 -20.3 0];
marking = [laneMarking('Solid', 'Color', [0.98 0.86 0.36])
    laneMarking('Dashed', 'Color', [0.98 0.86 0.36], 'Length', 5, 'Space', 5)
    laneMarking('Solid')];
laneSpecification = lanespec(2, 'Width', 3.925, 'Marking', marking);
road(scenario, roadCenters, 'Lanes', laneSpecification);

% Add the ego car
egoCar = vehicle(scenario, ...
    'ClassID', 1, ...
    'Position', [17.6 15 0]);
waypoints = [17.6 15 0;
    26.3 14.7 0;
    34.6 11.1 0;
    39.9 6.6 0;
    44.2 -0.9 0;
    45.1 -10.1 0;
    44.3 -17.9 0];
speed = [30;30;30;30;30;30;30];
trajectory(egoCar, waypoints, speed);

% Add the non-ego actors
car1 = vehicle(scenario, ...
    'ClassID', 1, ...
    'Position', [48.8 -17.9 0]);
waypoints = [48.8 -17.9 0;
    49.1 -9.4 0;
    49 -5 0;
    47.9 1.3 0;
    46.5 5.8 0;
    44.2 9.5 0;
    40.6 12.7 0;
    34.1 16 0;
    25.1 18.9 0;
    17.7 19.4 0];
speed = [30;30;30;30;30;30;30;30;30;30];
trajectory(car1, waypoints, speed);

truck = vehicle(scenario, ...
    'ClassID', 2, ...
    'Length', 8.2, ...
    'Width', 2.5, ...
    'Height', 3.5, ...
    'Position', [48.7 -7.8 0]);
waypoints = [48.7 -7.8 0;
    46.9 3.3 0;
    42 11.5 0;
    33.3 16.7 0;
    16.1 19.9 0];
speed = 30;
trajectory(truck, waypoints, speed);

