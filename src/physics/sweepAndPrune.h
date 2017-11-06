#pragma once

#include "common.h"
#include "utility/vector.h"

#define SAP_CLIENTID Uint16
#define SAP_POINTID Uint16
#define SAP_MAX_CLIENTS 8191

typedef struct SAPPoint {
    float value;
    SAP_CLIENTID parent;
    Uint8 isMax;
} SAPPoint;

typedef struct SAPClient {
    int entityID;
    SAP_POINTID xminRef;
    SAP_POINTID xmaxRef;
    float ymin;
    float ymax;
} SAPClient;

typedef struct SAPCandidate {
    int entityID;
    SAP_CLIENTID id;
} SAPCandidate;

typedef struct SAPCandidatePair {
    SAPCandidate c1;
    SAPCandidate c2;
    Uint8 yValid;
} SAPCandidatePair;

typedef struct SAP {
    SAPPoint points[SAP_MAX_CLIENTS*2];
    SAP_POINTID pointsCount;
    SAPClient clients[SAP_MAX_CLIENTS];
    SAPCandidatePair candidates[SAP_MAX_CLIENTS];
    int candidatesCount;
} SAP;

SAP* sap_create();

int sap_addClient(SAP* sap, int entityID, Vec2 pos, Vec2 sz);
void sap_addCandidate(SAP* sap, SAP_CLIENTID a, SAP_CLIENTID b);
void sap_updateClient(SAP* sap, SAP_CLIENTID clientID, Vec2 pos, Vec2 sz);
void sap_sortPoints(SAP* sap);
void sap_removePoint(SAP* sap, SAP_POINTID id);
void sap_removeClient(SAP* sap, SAP_CLIENTID clientID);
void sap_removeCandidatePair(SAP* sap, SAP_CLIENTID a, SAP_CLIENTID b);
void sap_removeCandidateID(SAP* sap, int candidateID);
void sap_removeCandidate(SAP* sap, SAP_CLIENTID a);

void sap_destroy(SAP* sap);

