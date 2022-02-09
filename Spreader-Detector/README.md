# Spreader-Detector
A C implementation of a program which evaluates the likelihood of a person getting infected by the corona virus, according to a given data.

## Table of contents
* [Description](#description)
* [Input](#input)
* [Output](#output)
* [Examples](#examples)

## Description
The corona plague reached Israel in February 2020 and began to spread at a dizzying pace. malls, restaurants and enclosed spaces
have been discovered As the main centers of infection and had to close immediately. In July of that year, these places began to gradually reopen
Their business but without a real solution as to dealing with potential infected people in those closed spaces, and so the State of Israel
decided it's best to have a lock down During the month of September for the entire holiday period, in order to somehow stop or atleast
reduce the spread of the corona virus.

The Spreader Detector program is a sophisticated and multi-system program designed to detect potential contagion chains,
evaluate the chance of a person being infected and update the potential infected person as fast and effective as possible.

The program works as follows:
* The Spreader Detector gets videos of people who have been identified as having the potential to be Corona carriers.
* The program then extracts the information from the videos as follows: who did the carriers meet with, who did those who the carriers met with - meet with,
  and so on until they identify all the people who appeared in the videos and know who met whom and when.
* The program transfers the information to a suitable database.

## Input
* People txt file, each line is of the form:
``` 
  <Person Name> <Person ID> <Person age> <SICK ?>\n
```
* Meetings txt file, each line is of the form:
```
  <person1_id> <person2_id> <distance> <measure>\n
```
  where distance and measure describe the distance the people who met and the duration of the meeting.

## Output
* a txt file which describes whether each person ought to be hospitalized, quarantied or doesn't need any treatment,
  where each line is according to the next format:
```
  <Status>: <Person Name> <Person ID> <Person Age> <Infection Rate>\n
```
  where the status is a string of the form: {"Hospitalization", "Quarantine", "No-Treatment"}

## Examples:
* People file:
```
    Ryan 533885365 90
    Marcus 992150860 80
    Kyle 919462684 70
    Vernon 353304652 60
    Paula 620434839 100 SICK
    Dorothy 673174908 50
```
* Meetings file:
```
    620434839 533885365 10 10
    620434839 673174908 10 10
    533885365 919462684 10 10
    533885365 992150860 10 10
    992150860 353304652 10 10
```
* Output file:
```
    Hospitalization: Paula 620434839 100 1.000000.
    Quarantine: Ryan 533885365 90 0.102222.
    No-Treatment: Marcus 992150860 80 0.082272.
    No-Treatment: Kyle 919462684 70 0.082272.
    No-Treatment: Vernon 353304652 60 0.081828.
    No-Treatment: Dorothy 673174908 50 0.022222.
```
