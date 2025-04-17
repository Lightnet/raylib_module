
```c
// Function to check if the model exists/loaded
bool IsModelValid(ModelComponent* component) {
  if (component == NULL || !component->isLoaded) {
      return false;
  }
  // Additional check: ensure model has valid mesh data
  return component->model.meshCount > 0 && component->model.meshes != NULL;
}

// // Simulate loading a model
// myModel.model = LoadModel("path/to/model.obj");
// myModel.isLoaded = true; // Mark as loaded
// // Check if model is valid
// if (IsModelValid(&myModel)) {
//     DrawModel(myModel.model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
// } else {
//     DrawText("Model is not valid", 10, 10, 20, RED);
// }
```