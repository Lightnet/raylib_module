

 * https://www.youtube.com/watch?v=BFxndyX0GhM


```
vector2 orignalPos = player->position

if(IsKeyDown(KEY_A)){
  player->position.x -= PLAYER_SPEED;
  if(IsCollision(player,blocks)){
    player->position.x = orignalPos.x;
  }
}


```