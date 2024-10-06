/*-------------------------------------------
Food Web Analysis with Dynamic Memory
------------------------------------------- */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Org_struct {
  char name[20];
  int *prey; // dynamic array of indices
  int numPrey;
} Org;

void buildWeb(Org *web, int numOrg, int predInd, int preyInd) {
  if (web[predInd].prey == NULL) {
    // If the prey array is empty, allocate memory for one index
    web[predInd].prey = (int *)malloc(sizeof(int));
    web[predInd].prey[0] = preyInd;
    web[predInd].numPrey = 1;
  } else {
    // If the prey array is not empty, reallocate memory to allow one more index
    int *temp = (int *)malloc(sizeof(int) * (web[predInd].numPrey + 1));

    // Copy existing prey indices to temp array
    for (int i = 0; i < web[predInd].numPrey; i++) {
      temp[i] = web[predInd].prey[i];
    }

    // Add the new prey index to the temp array
    temp[web[predInd].numPrey] = preyInd;
    web[predInd].numPrey++;

    // Free the old prey array and update with the new one
    free(web[predInd].prey);
    web[predInd].prey = temp;
  }
}

void extinction(Org **web, int *numOrgs, int index) {

  free((*web)[index].prey); // Free the prey array of the organism at index

  // Iterate through all organisms in the web
  for (int i = 0; i < *numOrgs; i++) {
    bool isExtinct = false;

    // Check if the current organism preys on the organism at index
    for (int j = 0; j < (*web)[i].numPrey; j++) {
      if ((*web)[i].prey[j] == index) {
        isExtinct = true;
      }
    }

    // If the organism preys on the extinct organism
    if (isExtinct) {
      // Allocate memory for an updated prey array with one less prey (excluding
      // the extinct organism)
      int *updatedPrey = (int *)malloc(sizeof(int) * ((*web)[i].numPrey - 1));
      int newPreyIndex = 0;

      // Update the prey array, excluding the extinct organism
      for (int j = 0; j < (*web)[i].numPrey; j++) {
        if ((*web)[i].prey[j] != index) {
          updatedPrey[newPreyIndex] = (*web)[i].prey[j];
          newPreyIndex++;
        }
      }

      free((*web)[i].prey);         // Free the old prey array
      (*web)[i].prey = updatedPrey; // Update the prey array
      (*web)[i].numPrey--;          // Update the number of prey

      if ((*web)[i].numPrey == 0) {
        (*web)[i].prey = NULL; // Set the prey array to NULL if there are no prey left
      }
    } 
  }

  // Allocate memory for an updated web with one less organism (the extinct
  // organism)
  Org *updatedWeb = (Org *)malloc((*numOrgs - 1) * sizeof(Org));
  int newOrgIndex = 0;

  // Update the web, excluding the extinct organism
  for (int i = 0; i < *numOrgs; i++) {
    if (i != index) {
      updatedWeb[newOrgIndex] = (*web)[i];

      // Update the prey indices that are greater than the extinct organism's
      // index
      for (int j = 0; j < updatedWeb[newOrgIndex].numPrey; j++) {
        if (updatedWeb[newOrgIndex].prey[j] > index) {
          updatedWeb[newOrgIndex].prey[j]--;
        }
      }
      newOrgIndex++;
    }
  }

  (*numOrgs)--;        // Decrement the number of organisms
  free(*web);          // Free the old web
  (*web) = updatedWeb; // Update the web pointer

  if ((*numOrgs) == 0) {
    (*web) = NULL; // Set the web pointer to NULL if there are no organisms left
  }
}

// Purpose is to print the food web, indicating organisms and their prey
// relationships. Takes in Org* web - a pointer to an array of organisms and int
// numOrgs - number of organisms in the array. Returning nothing (is a void).
void printFoodWeb(Org *web, int numOrgs) {
  // Iterate through each organism in the food web
  for (int i = 0; i < numOrgs; i++) {
    if (web[i].prey == NULL) {
      // Print the organism's name if it has no prey
      printf("  %s", web[i].name);
    } else if (web[i].numPrey == 1) {
      // Print the organism's name and the name of its prey if it has one prey
      printf("  %s eats %s", web[i].name, web[web[i].prey[0]].name);
    } else {
      // Print the organism's name and the names of its prey if it has multiple
      // prey
      printf("  %s eats %s", web[i].name, web[web[i].prey[0]].name);
      for (int j = 1; j < web[i].numPrey; j++) {
        printf(", %s", web[web[i].prey[j]].name);
      }
    }
    printf("\n"); // Move to the next line for the next organism
  }
}

// Purpose is to print organisms that do not have any predators in the food web.
// Takes in Org* web - a pointer to an array of organisms and int numOrgs -
// number of organisms in the array. Returning nothing (is a void).
void printApexPredators(Org *web, int numOrgs) {
  // Initialize an array to mark whether an organism is a prey
  bool isPrey[numOrgs];

  // Initialize all elements in isPrey to false
  for (int i = 0; i < numOrgs; i++) {
    isPrey[i] = false;
  }

  // Mark organisms that are prey in the isPrey array
  for (int i = 0; i < numOrgs; i++) {
    for (int j = 0; j < web[i].numPrey; j++) {
      isPrey[web[i].prey[j]] = true;
    }
  }

  // Print organisms that are not prey (apex predators)
  for (int i = 0; i < numOrgs; i++) {
    if (isPrey[i] == false) {
      printf("  %s\n", web[i].name);
    }
  }
}

// Purpose is to print organisms with no prey. Takes in Org* web - a pointer to
// an array of organisms and int numOrgs - number of organisms in the array.
// Returning nothing (is a void).
void printProducers(Org *web, int numOrgs) {
  // Iterate through each organism
  for (int i = 0; i < numOrgs; i++) {
    // Check if the current organism is a producer (has no prey)
    if (web[i].numPrey == 0) {
      // Print the name of the producer
      printf("  %s\n", web[i].name);
    }
  }
}

// Purpose is to print organisms thathave the maximum number of prey. Takes in
// Org* web - a pointer to an array of organisms and int numOrgs - number of
// organisms in the array. Returning nothing (is a void).
void printMostFlexibleEaters(Org *web, int numOrgs) {
  // Initialize a variable to keep track of the maximum prey count
  int max = -1;

  // Find the maximum number of prey an organism has
  for (int i = 0; i < numOrgs; i++) {
    if (web[i].numPrey > max) {
      max = web[i].numPrey; // Update the maximum prey count if a higher count is found
    }
  }

  // Print the names of organisms with the maximum number of prey
  for (int i = 0; i < numOrgs; i++) {
    if (web[i].numPrey == max) {
      printf("  %s\n", web[i].name); // Print the name of organisms with the maximum prey count
    }
  }
}

// Purpose is to print organisms that are the prey for the most other organisms.
// Takes in Org* web - a pointer to an array of organisms and int numOrgs -
// number of organisms in the array. Returning nothing (is a void).
void printTastiestFood(Org *web, int numOrgs) {
  // Initialize variables to keep track of max prey count and corresponding organism index
  int maxPreyCount = -1;
  int maxPreyIndex[numOrgs];
  int nummaxPreyIndex = 0;

  // Iterate through each organism
  for (int i = 0; i < numOrgs; i++) {
    int preyCount = 0; // Count of prey for the current organism

    // Iterate through the prey of the current organism
    for (int j = 0; j < numOrgs; j++) {
      if (i != j) { // Exclude the organism itself
        for (int k = 0; k < web[j].numPrey; k++) {
          if (web[j].prey[k] == i) {
            preyCount++; // Increment prey count if the current organism is prey
            break; // No need to check further if the organism is found as prey
          }
        }
      }
    }

    // Check if the prey count is greater than the current maximum prey count
    if (preyCount > maxPreyCount) {
      maxPreyCount = preyCount;
      nummaxPreyIndex = 1;
      maxPreyIndex[0] = i;
    } else if (preyCount == maxPreyCount) {
      // If the prey count is equal to the current maximum prey count, add the index to the list
      maxPreyIndex[nummaxPreyIndex] = i;
      nummaxPreyIndex++;
    }
  }

  // Print the names of organisms with the maximum prey count
  for (int i = 0; i < nummaxPreyIndex; i++) {
    printf("  %s\n", web[maxPreyIndex[i]].name);
  }
}

// Purpose is to print the height of each organism in the food web based on
// their position in the predator-prey relatinoship. Takes in Org* web - a
// pointer to an array of organisms and int numOrgs - number of organisms in the
// array. Returning nothing (is a void).
void printFoodWebHeights(Org *web, int numOrgs) {
  int webHeight[numOrgs]; // Array to store the height of each organsim
  for (int i = 0; i < numOrgs; i++) {
    webHeight[i] = 0; // Initialize all organism heights to 0
  }

  bool isChanged = true; // Variable to track if there were any height changes

  // Continue until there are no more changes in heights
  while (isChanged) {
    isChanged = false; // Reset the change flag for this iteration

    // Iterate through each organism
    for (int i = 0; i < numOrgs; i++) {
      int maxPreyDist =
          -1; // Variable to store the maximum distance of prey from producer
      // Iterate through the prey of the current organism
      for (int j = 0; j < web[i].numPrey; j++) {
        int preyIndex = web[i].prey[j]; // Index of the prey
        if (webHeight[preyIndex] > maxPreyDist) {
          maxPreyDist =
              webHeight[preyIndex]; // Update max distance of prey from producer
        }
      }

      int newHeight = maxPreyDist + 1; // Calculate the new height

      // Update the height if there's a change, and set the change flag
      if (newHeight != webHeight[i]) {
        webHeight[i] = newHeight;
        isChanged = true;
      }
    }
  }
  // Print the height of each web
  for (int i = 0; i < numOrgs; i++) {
    printf("  %s: %d\n", web[i].name, webHeight[i]);
  }
}

// Purpose is to print organisms categorized by their feeding types: producers,
// herbivores, omnivores, and carnivores. Takes in Org* web - a pointer to an
// array of organisms and int numOrgs - number of organisms in the array.
// Returning nothing (is a void).
void printVoreTypes(Org *web, int numOrgs) {
  int vore[numOrgs]; // Array to store the feeding type of each organism

  // Initialize vore array: -1 for uninitialized, 0 for producers, 1 for
  // carnivores, 2 for herbivores, 3 for omnivores
  for (int i = 0; i < numOrgs; i++) {
    vore[i] = -1;
  }

  // Determine the feeding type for each organism
  for (int i = 0; i < numOrgs; i++) {
    // If an organism has no prey, it's a producer (vore = 0)
    if (web[i].prey == NULL) {
      vore[i] = 0;
    }
  }

  // Determine feeding types based on prey feeding types
  for (int i = 0; i < numOrgs; i++) {
    if (web[i].prey == NULL) {
      // Already marked as a producer
      vore[i] = 0;
    } else if (vore[web[i].prey[0]] == 0) {
      // If the first prey is a producer, this is a carnivore (vore = 1) or an
      // omnivore (vore = 3)
      vore[i] = 2;
      // Check if there are other prey types to determine if it's an omnivore
      for (int j = 1; j < web[i].numPrey; j++) {
        if (vore[web[i].prey[j]] != 0) {
          vore[i] = 3;
          break;
        }
      }
    } else {
      // If the first prey is not a producer, this is a herbivore (vore = 2) or an omnivore (vore = 3)
      vore[i] = 1;
      // Check if there are other prey types to determine if it's an omnivore
      for (int j = 1; j < web[i].numPrey; j++) {
        if (vore[web[i].prey[j]] == 0) {
          vore[i] = 3;
          break;
        }
      }
    }
  }

  // Print organisms based on their feeding types
  printf("  Producers:\n");
  for (int i = 0; i < numOrgs; i++) {
    if (vore[i] == 0) {
      printf("    %s\n", web[i].name);
    }
  }

  printf("  Herbivores:\n");
  for (int i = 0; i < numOrgs; i++) {
    if (vore[i] == 2) {
      printf("    %s\n", web[i].name);
    }
  }

  printf("  Omnivores:\n");
  for (int i = 0; i < numOrgs; i++) {
    if (vore[i] == 3) {
      printf("    %s\n", web[i].name);
    }
  }

  printf("  Carnivores:\n");
  for (int i = 0; i < numOrgs; i++) {
    if (vore[i] == 1) {
      printf("    %s\n", web[i].name);
    }
  }
}

int main(void) {

  int numOrgs;
  printf("Welcome to the Food Web Application\n");
  printf("--------------------------------\n");
  printf("Enter number of organisms:\n");
  scanf("%d", &numOrgs);

  Org *web = NULL;
  if (numOrgs > 0) { // Do not malloc an empty array, leave it pointing to NULL
    web = (Org *)malloc(numOrgs * sizeof(Org));
  }

  printf("Enter names for %d organisms:\n", numOrgs);
  for (int i = 0; i < numOrgs; ++i) {
    scanf("%s", web[i].name);
    web[i].prey = NULL;
    web[i].numPrey = 0;
  }

  printf("Enter number of predator/prey relations:\n");
  int numRels;
  scanf("%d", &numRels);

  printf("Enter the pair of indices for the %d predator/prey relations\n",
         numRels);
  printf("the format is [predator index] [prey index]:\n");

  int predInd, preyInd;
  for (int i = 0; i < numRels; ++i) {
    scanf("%d %d", &predInd, &preyInd);
    buildWeb(web, numOrgs, predInd, preyInd);
  }
  printf("--------------------------------\n");

  printf("Food Web Predators & Prey:\n");
  printFoodWeb(web, numOrgs);
  printf("\n");

  printf("Apex Predators:\n");
  printApexPredators(web, numOrgs);
  printf("\n");

  printf("Producers:\n");
  printProducers(web, numOrgs);
  printf("\n");

  printf("Most Flexible Eaters:\n");
  printMostFlexibleEaters(web, numOrgs);
  printf("\n");

  printf("Tastiest Food:\n");
  printTastiestFood(web, numOrgs);
  printf("\n");

  printf("Food Web Heights:\n");

  printFoodWebHeights(web, numOrgs);
  printf("\n");

  printf("Vore Types:\n");
 
  printVoreTypes(web, numOrgs);
  printf("\n");

  printf("--------------------------------\n");
  int extInd;
  printf("Enter extinct species index:\n");
  scanf("%d", &extInd);
  printf("Species Extinction: %s\n", web[extInd].name);
  extinction(&web, &numOrgs, extInd);
  printf("--------------------------------\n\n");

  printf("UPDATED Food Web Predators & Prey:\n");

  printFoodWeb(web, numOrgs);
  printf("\n");

  printf("UPDATED Apex Predators:\n");

  printApexPredators(web, numOrgs);
  printf("\n");

  printf("UPDATED Producers:\n");

  printProducers(web, numOrgs);
  printf("\n");

  printf("UPDATED Most Flexible Eaters:\n");

  printMostFlexibleEaters(web, numOrgs);
  printf("\n");

  printf("UPDATED Tastiest Food:\n");

  printTastiestFood(web, numOrgs);
  printf("\n");

  printf("UPDATED Food Web Heights:\n");

  printFoodWebHeights(web, numOrgs);
  printf("\n");

  printf("UPDATED Vore Types:\n");

  printVoreTypes(web, numOrgs);
  printf("\n");
  printf("--------------------------------\n");

  for (int i = 0; i < numOrgs; i++) {
    free(web[i].prey);
  }
  free(web);

  return 0;
}
