#include "sweepAndPrune.h"

SAP* sap_create()
{
    SAP* sap = (SAP*)malloc(sizeof(SAP));
    sap->pointsCount = 0;
    sap->candidatesCount = 0;
    SAP_CLIENTID i;
    for(i = 0; i < SAP_MAX_CLIENTS; i++) sap->clients[i].entityID = -1;

    return sap;
}

int sap_addClient(SAP* sap, int entityID, Vec2 pos, Vec2 sz)
{
    SAPClient t = {entityID, sap->pointsCount, sap->pointsCount+1u, pos.y, pos.y + sz.y};

    SAP_CLIENTID i;
    for(i = 0; i < SAP_MAX_CLIENTS; i++)
    {
        if(sap->clients[i].entityID == -1)
        {
            sap->clients[i] = t;

            SAPPoint xmin = {pos.x, i, 0}, xmax = {pos.x + sz.x, i, 1};
            sap->points[sap->pointsCount] = xmin;
            sap->points[sap->pointsCount+1] = xmax;
            sap->pointsCount += 2;

            return i;
        }
    }
    fprintf(stderr, "Error : Not enough SAP client slots\n");
    return -1;
}

void sap_addCandidate(SAP* sap, SAP_CLIENTID a, SAP_CLIENTID b)
{
    //if(!m_clients[a].isDynamic and !m_clients[b].isDynamic) return;     //We are not interested in static pairs
    SAPCandidatePair cPair = {{sap->clients[a].entityID, a}, {sap->clients[b].entityID, b}, 1};
    sap->candidates[sap->candidatesCount] = cPair;
    sap->candidatesCount++;
}

void sap_updateClient(SAP* sap, SAP_CLIENTID clientID, Vec2 pos, Vec2 sz)
{
    sap->points[sap->clients[clientID].xminRef].value = pos.x;
    sap->points[sap->clients[clientID].xmaxRef].value = pos.x + sz.x;
    sap->clients[clientID].ymin = pos.y;
    sap->clients[clientID].ymax = pos.y + sz.y;
}

void sap_sortPoints(SAP* sap)
{
    SAP_POINTID i, k;
    for(i = 1; i < sap->pointsCount; i++)
    {
        for(k = i; k > 0 && (sap->points[k].value < sap->points[k-1].value); k--)
        {
            if(sap->points[k].isMax && !sap->points[k-1].isMax)                      //Adding or removing pairs if necessary
                sap_removeCandidatePair(sap, sap->points[k].parent, sap->points[k-1].parent);
            else if(!sap->points[k].isMax && sap->points[k-1].isMax)
                sap_addCandidate(sap, sap->points[k].parent, sap->points[k-1].parent);

            if(sap->points[k].isMax)
                sap->clients[sap->points[k].parent].xmaxRef--;
            else
                sap->clients[sap->points[k].parent].xminRef--;    //Updating refs

            if(sap->points[k-1].isMax)
                sap->clients[sap->points[k-1].parent].xmaxRef++;
            else
                sap->clients[sap->points[k-1].parent].xminRef++;

            SAPPoint t = sap->points[k];           //Switching points
            sap->points[k] = sap->points[k-1];
            sap->points[k-1] = t;
        }
    }

    //Sorting on y
    for(i = 0; i < sap->candidatesCount; i++)
    {
        if(sap->clients[sap->candidates[i].c1.id].ymax < sap->clients[sap->candidates[i].c2.id].ymin ||
           sap->clients[sap->candidates[i].c2.id].ymax < sap->clients[sap->candidates[i].c1.id].ymin)
            sap->candidates[i].yValid = 0;
        else
            sap->candidates[i].yValid = 1;
    }
}

void sap_removeClient(SAP* sap, SAP_CLIENTID clientID)
{
    sap_removePoint(sap, sap->clients[clientID].xminRef);
    sap_removePoint(sap, sap->clients[clientID].xmaxRef);

    sap_removeCandidate(sap, clientID);

    sap->clients[clientID].entityID = -1;
}

void sap_removePoint(SAP* sap, SAP_POINTID id)
{
    SAP_POINTID i;
    for(i = id; i < sap->pointsCount-1; i++)
    {
        sap->points[i] = sap->points[i+1];
        if(sap->points[i].isMax)
            sap->clients[sap->points[i].parent].xmaxRef--;
        else
            sap->clients[sap->points[i].parent].xminRef--;
    }
    sap->pointsCount--;
}

void sap_removeCandidatePair(SAP* sap, SAP_CLIENTID a, SAP_CLIENTID b)
{
    //if(!m_clients[a].isDynamic and !m_clients[b].isDynamic) return;     //We are not interested in static pairs
    int i = 0;
    while((sap->candidates[i].c1.id != a || sap->candidates[i].c2.id != b) &&
          (sap->candidates[i].c1.id != b || sap->candidates[i].c2.id != a) &&
          i < sap->candidatesCount) i++;

    sap->candidates[i] = sap->candidates[--sap->candidatesCount];
}

void sap_removeCandidateID(SAP* sap, int candidateID)
{
    sap->candidates[candidateID] = sap->candidates[--sap->candidatesCount];
}

void sap_removeCandidate(SAP* sap, SAP_CLIENTID a)
{
    int i;
    for(i = 0; i < sap->candidatesCount; i++)
    {
        if(sap->candidates[i].c1.id == a  || sap->candidates[i].c2.id == a)
        {
            sap_removeCandidateID(sap, i);
            i--;
        }
    }
}

void sap_destroy(SAP* sap)
{
    free(sap);
}
