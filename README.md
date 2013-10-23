Betgame Drupal 7 module
=======================

**Absolutely unstable** work in progress.

Module descriptions
-------------------

![Module structure](https://raw.github.com/michaelmrotzek/betgame/master/doc/modulstructure.png "Modules, Submodules and dependencies")

### Betgame
`betgame`: Basis betgame module offering basis infrastructure and hooks.

### Matchplans
* `betgame_matchplan`: Module for managing abstract matchplans
  * *complete*
  * matchplans can be integrated by writing some PHP code (see `betgame_matchplan_example` module) 
* `betgame_matchplan_example`: Module including some basic matchplans
  * *in progress* 
  * simple round robin *complete*
  * FIFA World Cup *not started*
  * UEFA Champions League *not started*
  * German Bundesliga *not started*
* `betgame_matchplan_generator`: Module to generate custom tournament matchplans
  * *not started*
  * define your own tournaments e.g. round robin

### Competition
* `betgame_competition`: Manage competitions and display results on pages.
  * create and manage competitions based on a matchplan
     * *mostly finished* 
     * integrate e.g. date picker to edit match kickoff
     * maybe extend to eddit enblems of a team and show it on score boards
  * render competitions game plans and score tables
     * *in progress* 
     * score tables calculation *complete*
     * rendering competition overview page *in progress* 
     * render results and table by group *in progress*
     * rendering results and table by gameday *in progress*

### Prediction
* `betgame_prediction`: Module enhances competition module to enable predictions by users
  * *startet, early stage*
  * users may predict results of a competition and will ear points for predictions
  * generates ranking of user based on their current score
* `betgame_prediction_teams`: Module enhances prediction module to enable predictions of several users in a team
  * *not started*
