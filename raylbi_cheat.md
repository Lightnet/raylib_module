# ref camera:

```c
Vector3 vecPos =  MatrixGetPosition(t[i].worldMatrix);
//Vector3 Vector3Lerp(Vector3 v1, Vector3 v2, float amount);
rl_ctx->camera.position = Vector3Lerp(rl_ctx->camera.position, vecPos, 0.9f);//smooth camera
```