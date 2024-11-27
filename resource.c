#include "defs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Resource functions */

/**
 * Creates a new `Resource` object.
 *
 * Allocates memory for a new `Resource` and initializes its fields.
 * The `name` is dynamically allocated.
 *
 * @param[out] resource      Pointer to the `Resource*` to be allocated and initialized.
 * @param[in]  name          Name of the resource (the string is copied).
 * @param[in]  amount        Initial amount of the resource.
 * @param[in]  max_capacity  Maximum capacity of the resource.
 */
 void resource_create(Resource **resource, const char *name, int amount, int max_capacity) {
     // Allocate memory for the Resource structure
     *resource = (Resource *)malloc(sizeof(Resource));
     if (*resource == NULL) {
         fprintf(stderr, "Failed to allocate memory for Resource.\n");
         exit(EXIT_FAILURE);
     }

     // Allocate memory for the name and copy it
     (*resource)->name = (char *)malloc(strlen(name) + 1);
     if ((*resource)->name == NULL) {
         fprintf(stderr, "Failed to allocate memory for Resource name.\n");
         free(*resource); // Avoid memory leak
         exit(EXIT_FAILURE);
     }
     strcpy((*resource)->name, name);

     // Initialize the fields
     (*resource)->amount = amount;
     (*resource)->max_capacity = max_capacity;
 }

/**
 * Destroys a `Resource` object.
 *
 * Frees all memory associated with the `Resource`.
 *
 * @param[in,out] resource  Pointer to the `Resource` to be destroyed.
 */
void resource_destroy(Resource *resource) {
   if (resource != NULL) {
        free(resource->name);
        free(resource);
    }
}

/* ResourceAmount functions */

/**
 * Initializes a `ResourceAmount` structure.
 *
 * Associates a `Resource` with a specific `amount`.
 *
 * @param[out] resource_amount  Pointer to the `ResourceAmount` to initialize.
 * @param[in]  resource         Pointer to the `Resource`.
 * @param[in]  amount           The amount associated with the `Resource`.
 */
void resource_amount_init(ResourceAmount *resource_amount, Resource *resource, int amount) {
    resource_amount->resource = resource;
    resource_amount->amount = amount;
}

/**
 * Initializes the `ResourceArray`.
 *
 * Allocates memory for the array of `Resource*` pointers and sets initial values.
 *
 * @param[out] array  Pointer to the `ResourceArray` to initialize.
 */
void resource_array_init(ResourceArray *array) {
    // Dynamically allocate memory for the array of Resource pointers
    array->resources = (Resource **)malloc(sizeof(Resource *) * 1);
    if (array->resources == NULL) {
        fprintf(stderr, "Failed to allocate memory for ResourceArray.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize array fields
    array->size = 0;
    array->capacity = 1;
}

/**
 * Cleans up the `ResourceArray` by destroying all resources and freeing memory.
 *
 * Iterates through the array, calls `resource_destroy` on each `Resource`,
 * and frees the array memory.
 *
 * @param[in,out] array  Pointer to the `ResourceArray` to clean.
 */
void resource_array_clean(ResourceArray *array) {
    if (array == NULL || array->resources == NULL) {
        return; // Nothing to clean
    }

    // Iterate through the resources and destroy each one
    for (int i = 0; i < array->size; i++) {
        if (array->resources[i] != NULL) {
            resource_destroy(array->resources[i]); // Frees the resource and its name
        }
    }

    // Free the dynamically allocated array of pointers
    free(array->resources);

    // Reset the structure to indicate it's cleaned
    array->resources = NULL;
    array->size = 0;
    array->capacity = 0;
}
/*
 * Adds a `Resource` to the `ResourceArray`, resizing if necessary (doubling the size).
 *
 * Resizes the array when the capacity is reached and adds the new `Resource`.
 * Use of realloc is NOT permitted.
 * 
 * @param[in,out] array     Pointer to the `ResourceArray`.
 * @param[in]     resource  Pointer to the `Resource` to add.
 */
 void resource_array_add(ResourceArray *array, Resource *resource) {
     // Check if resource or array is NULL
     if (array == NULL || resource == NULL) {
         fprintf(stderr, "Error: Null array or resource.\n");
         return;
     }

     // If size + 1 exceeds capacity, increase capacity
     if (array->size + 1 > array->capacity) {
         int new_capacity = (array->capacity == 0) ? 1 : array->capacity * 2;

         // Reallocate memory for the array
         Resource **new_array = realloc(array->resources, new_capacity * sizeof(Resource *));
         if (new_array == NULL) {
             fprintf(stderr, "Error: Memory allocation failed.\n");
             return;
         }

         array->resources = new_array;
         array->capacity = new_capacity;
     }

     // Add the resource to the array
     array-> resources[array->size] = resource;

     // Increment the size of the array
     array->size++;
 }