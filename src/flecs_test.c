// parent
// https://www.flecs.dev/flecs/md_docs_2Relationships.html#iterate-all-children-for-a-parent
//
#include "flecs.h"


int main(){
  // Initialize Flecs world
  ecs_world_t *world = ecs_init();

  ecs_entity_t parent = ecs_entity(world, {.name = "Parent"}); 
  ecs_entity_t child = ecs_entity(world, {.name = "Child"});
  // Create the hierarchy

  ecs_add_pair(world, child, EcsChildOf, parent);
  if (ecs_has_pair(world, child, EcsChildOf, parent)){
    ecs_print(1,"found ecs_has_pair");
  }

  if (parent == ecs_lookup(world, "Parent.Child")) {
    // true
    ecs_print(1,"found");
  }


  //=============================================
  // working
  //=============================================

  // ecs_entity_t p = ecs_entity(world, { .name = "Parent" });
  // ecs_entity_t e = ecs_entity(world, { .name = "Child", .parent = p });
   
  // if (e == ecs_lookup(world, "Parent.Child")) {
  //     // true
  //     ecs_print(1,"found");
  // }
  // if (e == ecs_lookup_from(world, p, "Child")) {
  //   // true
  //   ecs_print(1,"found");
  // }



  ecs_fini(world);

  //printf("finish\n");
  ecs_print(1,"finish");

  return 0;
}