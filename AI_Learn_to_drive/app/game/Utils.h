#pragma once

#include <raylib.h>
#include <cmath>

#include "Track.h"

float CastRay(Vector2 carPos, float rayAngle, float maxRange, Track& track);

float DegreeToRadian(float angle);