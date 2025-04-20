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

  ecs_progress(world, 0);

  // working 
  if (ecs_has_pair(world, child, EcsChildOf, parent)){
    ecs_print(1,"found ecs_has_pair");
  }

  if (parent == ecs_lookup(world, "Parent")) {
    // true
    ecs_print(1,"ecs_lookup Parent found");
  }
  //nope //not correct// this look for child name not parent
  if (parent == ecs_lookup(world, "Parent.Child")) {
    ecs_print(1,"ecs_lookup found parent");
  }

  // true
  if (child == ecs_lookup_from(world, parent, "Child")) {
    ecs_print(1,"ecs_lookup_from found Child");
  }
  // nope //not correct but check for compare
  if (parent == ecs_lookup_from(world, parent, "Child")) {
    ecs_print(1,"ecs_lookup_from found");
  }
  
  //true
  if (child == ecs_lookup(world, "Parent.Child")) {
    ecs_print(1,"ecs_lookup found . child");
  }
  //nope
  if (child == ecs_lookup(world, "Parent::Child")) {
    ecs_print(1,"ecs_lookup found :: child");
  }
  //working
  const char *nameparent = ecs_get_name(world, parent);
  ecs_print(1,"NAME: %s", nameparent); // Parent

  ecs_entity_t l_parent = ecs_get_parent(world, child);
  if(l_parent){//check remove pair test
    const char *nameparent02 = ecs_get_name(world, l_parent);
    ecs_print(1,"get parent NAME: %s", nameparent02); // Parent
  }

  // Returns path, result must be freed
  char *path = ecs_get_path(world, child);
  ecs_print(1,"PATH: %s", path); // Parent.Child
  ecs_os_free(path);

  ecs_print(1,"parent children count");
  ecs_iter_t it = ecs_children(world, parent);
  while (ecs_children_next(&it)) {
    ecs_print(1,"Children Count: %d", it.count);
    for (int i = 0; i < it.count; i ++) {
      ecs_entity_t child = it.entities[i];
      const char *namechild = ecs_get_name(world, child);
      ecs_print(1,"Name Child: %s", namechild);
    }
  }
  ecs_print(1,"remove child from parent");
  ecs_remove_pair(world, child, EcsChildOf, parent);
  ecs_print(1,"parent children count");
  it = ecs_children(world, parent);
  while (ecs_children_next(&it)) {
    ecs_print(1,"Children Count: %d", it.count);
    for (int i = 0; i < it.count; i ++) {
      ecs_entity_t child = it.entities[i];
      const char *namechild = ecs_get_name(world, child);
      ecs_print(1,"Name Child: %s", namechild);
    }
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