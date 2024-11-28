#include "defs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// Global resource
sem_t resource_sem;


void load_data(Manager *manager);

int main(void) {
    // Initialize semaphore
    if (sem_init(&resource_sem, 0, 1) != 0) {
        perror("Failed to initialize semaphore");
        exit(EXIT_FAILURE);
    }

    Manager manager;
    manager_init(&manager);
    load_data(&manager);

    // Create manager thread
    pthread_t manager_tid;
    if (pthread_create(&manager_tid, NULL, manager_thread, (void*)&manager) != 0) {
        perror("Failed to create manager thread");
        sem_destroy(&resource_sem);
        manager_clean(&manager);
        exit(EXIT_FAILURE);
    }

    // Create system threads
    int num_systems = manager.system_array.size;
    pthread_t system_tids[num_systems];

    for (int i = 0; i < num_systems; ++i) {
        if (pthread_create(&system_tids[i], NULL, system_thread, (void*)manager.system_array.systems[i]) != 0) {
            perror("Failed to create system thread");
            // Signal termination to already created threads
            manager.simulation_running = 0;
            // Wait for already created threads to terminate
            for (int j = 0; j < i; ++j) {
                pthread_join(system_tids[j], NULL);
            }
            pthread_join(manager_tid, NULL);
            sem_destroy(&resource_sem);
            manager_clean(&manager);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for manager thread to finish
    pthread_join(manager_tid, NULL);

    // Wait for all system threads to finish
    for (int i = 0; i < num_systems; ++i) {
        pthread_join(system_tids[i], NULL);
    }

    // Cleanup
    sem_destroy(&resource_sem);
    manager_clean(&manager);

    printf("Simulation terminated and resources cleaned up.\n");
    return 0;
}


// int main(void) {
//     Manager manager;
//     manager_init(&manager);
//     load_data(&manager);
//
//     while (manager.simulation_running) {
//         manager_run(&manager);
//         for (int i = 0; i < manager.system_array.size; ++i) {
//             system_run(manager.system_array.systems[i]);
//         }
//     }
//
//     manager_clean(&manager);
//     return 0;
// }
//
/**
 * Loads sample data for the simulation.
 *
 * Calls all of the functions required to create resources and systems and add them to the Manager's data.
 *
 * @param[in,out] manager  Pointer to the `Manager` to populate with resource and system data.
 */
void load_data(Manager *manager) {
    // Create resources
    Resource *fuel, *oxygen, *energy, *distance;
    resource_create(&fuel, "Fuel", 1000, 1000);
    resource_create(&oxygen, "Oxygen", 20, 50);
    resource_create(&energy, "Energy", 30, 50);
    resource_create(&distance, "Distance", 0, 5000);

    resource_array_add(&manager->resource_array, fuel);
    resource_array_add(&manager->resource_array, oxygen);
    resource_array_add(&manager->resource_array, energy);
    resource_array_add(&manager->resource_array, distance);

    // Create systems
    System *propulsion_system, *life_support_system, *crew_capsule_system, *generator_system;
    ResourceAmount consume_fuel, produce_distance;
    resource_amount_init(&consume_fuel, fuel, 5);
    resource_amount_init(&produce_distance, distance, 25);
    system_create(&propulsion_system, "Propulsion", consume_fuel, produce_distance, 50, &manager->event_queue);

    ResourceAmount consume_energy, produce_oxygen;
    resource_amount_init(&consume_energy, energy, 7);
    resource_amount_init(&produce_oxygen, oxygen, 4);
    system_create(&life_support_system, "Life Support", consume_energy, produce_oxygen, 10, &manager->event_queue);

    ResourceAmount consume_oxygen, produce_nothing;
    resource_amount_init(&consume_oxygen, oxygen, 1);
    resource_amount_init(&produce_nothing, NULL, 0);
    system_create(&crew_capsule_system, "Crew", consume_oxygen, produce_nothing, 2, &manager->event_queue);

    ResourceAmount consume_fuel_for_energy, produce_energy;
    resource_amount_init(&consume_fuel_for_energy, fuel, 5);
    resource_amount_init(&produce_energy, energy, 10);
    system_create(&generator_system, "Generator", consume_fuel_for_energy, produce_energy, 20, &manager->event_queue);

    system_array_add(&manager->system_array, propulsion_system);
    system_array_add(&manager->system_array, life_support_system);
    system_array_add(&manager->system_array, crew_capsule_system);
    system_array_add(&manager->system_array, generator_system);
}