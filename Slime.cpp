#include<iostream>
#include<list>
#include<algorithm>
#include"Vector2.h"
#include"Vector3.h"
#include"DataMap.h"
#include"Slime.h"

list<Slime> Slime::AllSlimes;
DataMap Slime::map;
int Slime::SCREEN_WIDTH;
int Slime::SCREEN_HEIGHT;

int Slime::SQUARES_WIDTH;
int Slime::SQUARES_HEIGHT;

Slime::Slime(Vector2 position_, Vector2 vector_, float speed_) 
{
	position = position_;
	vector = vector_;
    speed = speed_;

    leftSensorVec = Vector2((float)cos(3.1415 * sensorAngle / 180), (float)sin(3.1415 * sensorAngle / 180));
    rightSensorVec = Vector2((float)cos(3.1415 * -sensorAngle / 180), (float)sin(3.1415 * -sensorAngle / 180));

    vectorLeft = Vector2((float)cos(3.1415 * turnPerStep / 180), (float)sin(3.1415 * turnPerStep / 180));
    vectorRight = Vector2((float)cos(3.1415 * -turnPerStep / 180), (float)sin(3.1415 * -turnPerStep / 180));

    Slime::AllSlimes.push_back(*this);
}

void Slime::UpdateAll()
{
    std::list<Slime>::iterator it;
    std::for_each(Slime::AllSlimes.begin(), Slime::AllSlimes.end(), 
        [](Slime & slime)
        {
            slime.Update();
        });
}

void Slime::Update()
{
    for (int i = 0; i < speed; i++)
    {
        Sense();
        Move();
        Trail();
    }
}

void Slime::GetSensorCoords()
{
    Vector2 v0 = vector.GetRotatedOverVec(leftSensorVec);
    v0.Normalize();
    v0.ScalarMultiply(sensorDistance);
    sensor0 = position.Duplicate();
    sensor0.Add(v0);
    
    Vector2 v1 = vector.Duplicate();
    v1.Normalize();
    v1.ScalarMultiply(sensorDistance);
    sensor1 = position.Duplicate();
    sensor1.Add(v1);

    Vector2 v2 = vector.GetRotatedOverVec(rightSensorVec);
    v2.Normalize();
    v2.ScalarMultiply(sensorDistance);
    sensor2 = position.Duplicate();
    sensor2.Add(v2);
}

float Slime::GetPowerAtPosition(Vector2 position)
{
    return 1;
}

bool Slime::SensorOut(Vector2 sensor)
{
    if (sensor.x < 0 || sensor.x >= Slime::SCREEN_WIDTH || sensor.y < 0 || sensor.y >= Slime::SCREEN_HEIGHT)
    {
        return true;
    }
    return false;
}
void Slime::Sense()
{
    GetSensorCoords();

    bool outing = false;

    if (Slime::SensorOut(sensor1))
    {
        outing = true;
        // If already turning
        if (turningAwayFromWall != NULL)
        {
            if (turningAwayFromWall == 0)
            {
                TurnLeft();
            } else
            {
                TurnRight();
            }
        } else
        {
            turningAwayFromWall = 0;
            TurnLeft();
        }
    }
    else
    {
        if (Slime::SensorOut(sensor0))
        {
            outing = true;
            TurnRight();
        }
        if (Slime::SensorOut(sensor2))
        {
            outing = true;
            TurnLeft();
        }
    }

    if (!outing)
    {
        turningAwayFromWall = NULL;

        float sensorValue0 = Slime::GetPowerAtPosition(sensor0);
        float sensorValue1 = Slime::GetPowerAtPosition(sensor1);
        float sensorValue2 = Slime::GetPowerAtPosition(sensor2);

        //Front > both, do nothing
        if (sensorValue1 > sensorValue0 && sensorValue1 > sensorValue2)
        {
            
        }
        // Front < both, so trails are on both sides, choose one at random
        else if (sensorValue1 < sensorValue0 && sensorValue1 < sensorValue2)
        {
             TurnLeft();
        }
        // Front > one of them, check which one is greaterand turn that way
        // Right > Left
        else if (sensorValue2 > sensorValue0)
        {
            TurnRight();
        }
        // Left > Right
        else if (sensorValue2 > sensorValue0)
        {
            TurnLeft();
        }
    }
}

void Slime::TurnLeft()
{
    vector = vector.GetRotatedOverVec(vectorLeft);
    vector.Normalize();
}

void Slime::TurnRight()
{
    vector = vector.GetRotatedOverVec(vectorRight);
    vector.Normalize();
}

void Slime::Trail()
{
    int j = (int)(position.x * Slime::SQUARES_WIDTH / Slime::SCREEN_WIDTH);
    int i = (int)(position.y * Slime::SQUARES_HEIGHT / Slime::SCREEN_HEIGHT);
    Slime::map.map[i][j] = Vector3(1);
    Slime::map.map[i+1][j] = Vector3(1);
    Slime::map.map[i-1][j] = Vector3(1);
    Slime::map.map[i+1][j+1] = Vector3(1);
    Slime::map.map[i+1][j-1] = Vector3(1);
    Slime::map.map[i][j+1] = Vector3(1);
    Slime::map.map[i][j-1] = Vector3(1);
    Slime::map.map[i-1][j+1] = Vector3(1);
    Slime::map.map[i-1][j-1] = Vector3(1);
}

void Slime::Move()
{
    position.Add(vector);
    WallCollision();
}

void Slime::WallCollision()
{
    if (position.x < 0)
    {
        position.x = 0;
    }
    else if (position.x > SCREEN_WIDTH)
    {
        position.x > SCREEN_WIDTH;
    }
    if (position.y < 0)
    {
        position.y = 0;
    }
    else if (position.y > SCREEN_HEIGHT)
    {
        position.y = SCREEN_HEIGHT;
    }
}

void Slime::WallTeleport()
{
    if (position.x < 0)
    {
        position.x = SCREEN_WIDTH;
    } else if (position.x > SCREEN_WIDTH)
    {
        position.x > 0;
    }
    if (position.y < 0)
    {
        position.y = SCREEN_HEIGHT;
    } else if (position.y > SCREEN_HEIGHT)
    {
        position.y = 0;
    }
}


//void Slime::AddSlime(Slime s);
//void Slime::UpdateAll();
//void Slime::DrawAll();