<?php
require_once(drupal_get_path('module', 'betgame') . '/lib/EntityDao.php');
require_once(drupal_get_path('module', 'betgame_prediction') . '/lib/PredictionEntity.php');

class PredictionDao extends AbstractEntityDao {

  function entityTableMap() {
    return array(
      'Prediction' => 'betgame_prediction',
    );
  }
  
  // TODO refactor, clean api!!
  public function findRankings($competition_id, $algo = 1) {
    $this->updatePoints($competition_id);
    
    if($algo === 1) {
      return $this->calculateRankings1();
    }
    if($algo === 2) {
      return $this->calculateRankings2();
    }
  }

  public function updatePoints($competition_id) {
    $table = 'betgame_prediction_points';
    $field = 'score';
    
    db_query(
        "DELETE FROM betgame_prediction WHERE competitionid=".$competition_id."
        "
    );
    
    $query = db_query(
        "INSERT INTO betgame_prediction (score)
          SELECT SUM(" . $field . ")
          FROM {".$table."}
          WHERE competitionid=".$competition_id."
          GROUP BY userid
        "
    );
  }
  
  public function calculateRankings1() {
    $table = 'betgame_prediction_points';
    $field = 'score';
  
    $query = db_query(
        "SELECT COUNT(a2." . $field . ") rank, a1.*
        FROM {".$table."} a1, {".$table."} a2
        WHERE (a1." . $field . " < a2." . $field . " OR (a1." . $field . "=a2." . $field . " AND a1.userid = a2.userid))
        GROUP BY a1.id, a1." . $field . "
        ORDER BY a1." . $field . " DESC
        "
    );
    return $query->fetchAll();

  }
  
  protected function calculateRankings2() {
    $table = 'betgame_prediction_points';
    $field = 'score';
  
    db_query("SET @rank=0;");
    $query = db_query(
        "SELECT @rank:=@rank+1 AS rank, t.*, SUM(".$field.") as sum_score
        FROM {".$table."} t
        GROUP BY userid
        ORDER BY sum_score DESC"
    );
    return $query->fetchAll();
  }
  
  function getUserRank($competition_id, $userid) {
    $field = 'score';
    $table = 'betgame_prediction_points';
    
    $query = db_query(
        "SELECT COUNT(*) from 
        (
          SELECT SUM(t.".$field.") AS sum_score
          FROM {".$table."} t
          GROUP BY t.userid
        )
        WHERE sum_score > (SELECT SUM(score) FROM {".$table."} WHERE userid = @userid)
        "
    );
    return $query->fetchCol();
    
    /* list of all users */
  /* select count(*) + 1 from 
(
    SELECT SUM( p.points ) AS sum_points
    FROM user u
    LEFT JOIN points p ON p.user_id = u.id
    GROUP BY u.id        
) x
where sum_points > (select sum(points) from points where user_id = @this_user)
*/
/* just count the ones with higher sum_points */

  }
  
  public function storePrediction($competition_id, Prediction $prediction) {
    return $this->storeEntity($prediction);
  }
  
  public function findMatchPredictions($competition_id, $match_id) {
    $conditions[] = new Condition('competitionid', $competition_id);
    $conditions[] = new Condition('matchid', $match_id);
    $order = array();
    
    return $this->findEntities('Prediction', $conditions, $order);
  
  }
  
  public function findPredictions($competition_id, $user_id) {
    $conditions[] = new Condition('competitionid', $competition_id);
    $conditions[] = new Condition('userid', $user_id);
    
    $order = array(new Order('userid'));
    
    return $this->findEntities('Prediction', $conditions, $order);
  
  }
}