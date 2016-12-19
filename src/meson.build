gnome = import('gnome')

resources = gnome.compile_resources('resources',
                                    'recipes.gresource.xml',
                                    c_name: '_recipes',
                                    source_dir: 'src')

enums = gnome.mkenums('types',
                      sources: 'gr-recipe.h',
                      c_template: 'types.c.template',
                      h_template: 'types.h.template')


src =  ['main.c',
       'gr-app.c',
       'gr-big-cuisine-tile.c',
       'gr-category-tile.c',
       'gr-chef.c',
       'gr-chef-tile.c',
       'gr-cuisine.c',
       'gr-cuisine-page.c',
       'gr-cuisine-tile.c',
       'gr-cuisines-page.c',
       'gr-details-page.c',
       'gr-diet-row.c',
       'gr-edit-page.c',
       'gr-images.c',
       'gr-ingredient.c',
       'gr-ingredient-row.c',
       'gr-ingredient-tile.c',
       'gr-ingredients-list.c',
       'gr-ingredients-page.c',
       'gr-list-page.c',
       'gr-meal.c',
       'gr-meal-row.c',
       'gr-preferences.c',
       'gr-query-editor.c',
       'gr-recipe.c',
       'gr-recipe-printer.c',
       'gr-recipe-store.c',
       'gr-recipe-tile.c',
       'gr-recipes-page.c',
       'gr-search-page.c',
       'gr-season.c',
       'gr-timer.c',
       'gr-timer-widget.c',
       'gr-toggle-button.c',
       'gr-utils.c',
       'gr-window.c',
  enums,
  resources]

executable('recipe',
           src,
           install : true,
           include_directories : top_inc,
           dependencies: deps)

