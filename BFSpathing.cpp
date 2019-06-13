using namespace std;

#include "ConsoleEngine.h"

class BFSpathing : public ConsoleTemplateEngine {
public:
	BFSpathing() {
		m_sAppName = L"BFS Pathing";
	}

private:
	// Map tiles
	struct sNodes {
		int x;
		int y;
		bool bObstacle = false;
		int nWeight = 1;
		//int nDistanceFromCurrent = 0;
	};

	int nWidth = 0;
	int nHeight = 0;
	sNodes* map = nullptr;
	vector<sNodes*> vTilesInRange = {};

	// Player
	int nUserPosX = 0;
	int nUserPosY = 0;
	int nRange = 0;

	// Range Origin
	int nOldPosX = 0;
	int nOldPosY = 0;

	virtual bool OnUserCreate() {

		nWidth = 15;
		nHeight = 15;

		map = new sNodes[nWidth * nHeight];
		memset(map, 0, nWidth * nHeight * sizeof(sNodes));

		for (int i = 0; i < nWidth; i++) {
			for (int j = 0; j < nHeight; j++) {
				map[j * nWidth + i].x = i;
				map[j * nWidth + i].y = j;
				map[j * nWidth + i].nWeight = 1;
			}
		}

		nUserPosX = nWidth / 2;
		nUserPosY = nHeight / 2;
		nOldPosX = nUserPosX;
		nOldPosY = nUserPosY;
		nRange = 3;

		InRange(&vTilesInRange);

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		
		int nChangeY = 0;
		int nChangeX = 0;

		// Handle User Input

		if (IsFocused()) {
			if (GetKey(VK_RIGHT).bPressed) {
				nUserPosX += 1;
			}
			if (GetKey(VK_LEFT).bPressed) {
				nUserPosX -= 1;
			}
			if (GetKey(VK_DOWN).bPressed) {
				nUserPosY += 1;
			}
			if (GetKey(VK_UP).bPressed) {
				nUserPosY -= 1;
			}
			if (GetKey(VK_ADD).bPressed) {
				nRange += 1;
			}
			if (GetKey(VK_SUBTRACT).bPressed) {
				nRange -= 1;
			}
			if (GetKey(VK_RETURN).bPressed) {
				nOldPosX = nUserPosX;
				nOldPosY = nUserPosY;
				vTilesInRange.clear();
				InRange(&vTilesInRange);
			}
		}

		

			// Clamp User
		if (nUserPosX >= nWidth) {
			nUserPosX = nWidth - 1;
		}
		if (nUserPosX < 0) {
			nUserPosX = 0;
		}
		if (nUserPosY >= nHeight) {
			nUserPosY = nHeight - 1;
		}
		if (nUserPosY < 0) {
			nUserPosY = 0;
		}

		
		// Drawing variables
		int nTileWidth = 4;
		int nTileHeight = 4;
		int nOffset = 2;

		// Draw map
		for (int i = 0; i < nWidth; i++) {
			for (int j = 0; j < nHeight; j++) {
				Fill(i * nTileWidth + (i * nOffset),
					j * nTileHeight + (j * nOffset),
					(i + 1) * nTileWidth + (i * nOffset),
					(j + 1) * nTileHeight + (j * nOffset),
					PIXEL_HALF,
					FG_BLUE);

			}
		}

		// Draw Range
		for (auto& n : vTilesInRange) {
			Fill(n->x * nTileWidth + (n->x * nOffset),
				n->y * nTileHeight + (n->y * nOffset),
				(n->x + 1) * nTileWidth + (n->x * nOffset),
				(n->y + 1) * nTileHeight + (n->y * nOffset),
				PIXEL_SOLID,
				FG_BLUE);
		}

		// Draw user
		Fill(nUserPosX * nTileWidth + (nUserPosX * nOffset),
			nUserPosY * nTileHeight + (nUserPosY * nOffset),
			(nUserPosX + 1) * nTileWidth + (nUserPosX * nOffset),
			(nUserPosY + 1) * nTileHeight + (nUserPosY * nOffset),
			PIXEL_SOLID,
			FG_GREEN);

		return true;
	}

	void InRange(vector<sNodes*>* result) {
		bool* bVisited = new bool[nWidth * nHeight];
		int* nCurrentMovement = new int[nWidth * nHeight];

		vector<sNodes*> travelGuide;
		vector<sNodes*> navigated;
		sNodes* temp;

		for (int i = 0; i < nWidth; i++)
			for (int j = 0; j < nHeight; j++) {
				bVisited[j * nWidth + i] = false;
				nCurrentMovement[j * nWidth + i] = 0;
			}

		travelGuide.push_back(&map[nOldPosY * nWidth + nOldPosX]);
		bVisited[nOldPosY * nWidth + nOldPosX] = false;

		while (!travelGuide.empty()) {
			if (bVisited[travelGuide[0]->y * nWidth + travelGuide[0]->x]) {
				travelGuide.erase(travelGuide.begin());
				continue;
			}
			bVisited[travelGuide[0]->y * nWidth + travelGuide[0]->x] = true;
			navigated.push_back(travelGuide[0]);

			if (!travelGuide[0]->bObstacle && nCurrentMovement[travelGuide[0]->y * nWidth + travelGuide[0]->x] <= nRange) {
				result->push_back(travelGuide[0]);

				// Left Node
				if (travelGuide[0]->x - 1 >= 0) {
					temp = &map[travelGuide[0]->y * nWidth + travelGuide[0]->x - 1];
					nCurrentMovement[temp->y * nWidth + temp->x] = nCurrentMovement[travelGuide[0]->y * nWidth + travelGuide[0]->x] + temp->nWeight;
					travelGuide.push_back(temp);
				}
				// Right Node
				if (travelGuide[0]->x + 1 < nWidth) {
					temp = &map[travelGuide[0]->y * nWidth + travelGuide[0]->x + 1];
					nCurrentMovement[temp->y * nWidth + temp->x] = nCurrentMovement[travelGuide[0]->y * nWidth + travelGuide[0]->x] + temp->nWeight;
					travelGuide.push_back(temp);
				}
				// Top Node
				if (travelGuide[0]->y - 1 >= 0) {
					temp = &map[(travelGuide[0]->y - 1) * nWidth + travelGuide[0]->x];
					nCurrentMovement[temp->y * nWidth + temp->x] = nCurrentMovement[travelGuide[0]->y * nWidth + travelGuide[0]->x] + temp->nWeight;
					travelGuide.push_back(temp);
				}
				// Bot Node
				if (travelGuide[0]->y + 1 < nHeight) {
					temp = &map[(travelGuide[0]->y + 1) * nWidth + travelGuide[0]->x];
					nCurrentMovement[temp->y * nWidth + temp->x] = nCurrentMovement[travelGuide[0]->y * nWidth + travelGuide[0]->x] + temp->nWeight;
					travelGuide.push_back(temp);
				}
			}
			travelGuide.erase(travelGuide.begin());
		}

		delete[] bVisited;
		delete[] nCurrentMovement;

		return;

	}

	/*
	void InRange(vector<sNodes*>* accepted) {
        vector<sNodes*> travelGuide;
        vector<sNodes*> navigated;
        sNodes* temp;
        char buff[100];
        
        travelGuide.push_back(&map[nUserPosY * nWidth + nUserPosX]);
        travelGuide[0]->nDistanceFromCurrent = 0;

        while (!travelGuide.empty()) {
            if (travelGuide[0]->bVisited) {
                travelGuide.erase(travelGuide.begin());
                continue;
            }
            travelGuide[0]->bVisited = true;
            navigated.push_back(travelGuide[0]);

            if (!travelGuide[0]->bObstacle && travelGuide[0]->nDistanceFromCurrent <= nRange) {
                accepted->push_back(travelGuide[0]);

                // node to the left
                if (travelGuide[0]->x - 1 >= 0) {
                    temp = &map[travelGuide[0]->y * nWidth + travelGuide[0]->x - 1];
                    temp->nDistanceFromCurrent = travelGuide[0]->nDistanceFromCurrent + temp->nWeight;
                    travelGuide.push_back(temp);
                }
                // node to the right
                if (travelGuide[0]->x + 1 < nWidth) {
                    temp = &map[travelGuide[0]->y * nWidth + travelGuide[0]->x + 1];
                    temp->nDistanceFromCurrent = travelGuide[0]->nDistanceFromCurrent + temp->nWeight;
                    travelGuide.push_back(temp);
                }
                // node above
                if (travelGuide[0]->y - 1 >= 0) {
                    temp = &map[(travelGuide[0]->y - 1) * nWidth + travelGuide[0]->x];
                    temp->nDistanceFromCurrent = travelGuide[0]->nDistanceFromCurrent + temp->nWeight;
                    travelGuide.push_back(temp);
                }
                //node below
                if (travelGuide[0]->y + 1 < nHeight) {
                    temp = &map[(travelGuide[0]->y + 1) * nWidth + travelGuide[0]->x];
                    temp->nDistanceFromCurrent = travelGuide[0]->nDistanceFromCurrent + temp->nWeight;
                    travelGuide.push_back(temp);
                }
            }

            travelGuide.erase(travelGuide.begin());
        }

        for (auto& n : navigated) {
            n->bVisited = false;
        }
    }
	*/

};

int main() {
	BFSpathing demo;
	if (demo.ConstructConsole(160, 100, 8, 8)) {
		demo.Start();
	}

	return 0;
}