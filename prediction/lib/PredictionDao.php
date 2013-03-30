<?php
require_once(drupal_get_path('module', 'betgame') . '/lib/EntityDao.php');
require_once(drupal_get_path('module', 'betgame_prediction') . '/lib/PredictionEntity.php');

class PredictionDao extends AbstractEntityDao {

  private $table_prediction = 'betgame_prediction';
  private $table_results = 'betgame_prediction_points';

  function entityTableMap() {
    return array(
      'Prediction' => $this->table_prediction,
      'PredictionResult' => $this->table_results,
    );
  }

  /**
   * Update sums of users predictions scores.
   * 
   * @param int $competition_id
   */
  public function updateScores($competition_id) {
    $field = 'score';

    db_query(
        "DELETE FROM {" . $this->table_results . "} WHERE competitionid=".$competition_id."
        "
    );

    $query = db_query(
        "INSERT INTO {" . $this->table_results . "} (score, userid, competitionid)
        (SELECT SUM(" . $field . "), userid, competitionid
        FROM {" . $this->table_prediction . "}
        WHERE competitionid=".$competition_id."
        GROUP BY userid)
        "
    );
  }

  /**
   * Get prediction ranking of a competition.
   *
   * @param int $competition_id
   */
  public function findRankings($competition_id) {
    $table = $this->table_results;
    $field = 'score';

    $query = db_query(
        "SELECT COUNT(a2." . $field . ") rank, a1.*
        FROM {".$table."} a1, {".$table."} a2
        WHERE (a1." . $field . " < a2." . $field . " OR (a1." . $field . "=a2." . $field . " AND a1.userid = a2.userid))
          AND (a1.competitionid = " . $competition_id . " AND a1.competitionid = a2.competitionid)
        GROUP BY a1.id, a1." . $field . "
        ORDER BY a1." . $field . " DESC
        "
    );
    return $query->fetchAll();
  }

  /**
   * Gets a prediction rank of a user.
   * 
   * @param int $competition_id
   * @param int $userid
   */
  function getUserRanking($competition_id, $userid) {
    $table = $this->table_results;
    $field = 'score';
    
    $query = db_query(
        "SELECT COUNT(a2." . $field . ") rank
        FROM {".$table."} a1, {".$table."} a2
        WHERE (a1." . $field . " < a2." . $field . " OR (a1." . $field . "=a2." . $field . " AND a1.userid = a2.userid))
          AND (a1.competitionid = " . $competition_id . " AND a1.competitionid = a2.competitionid)
          AND a1.userid = " . $userid . "
        GROUP BY a1.id, a1." . $field . "
        ORDER BY a1." . $field . " DESC,
        "
    );
    
    $result = $query->fetchCol();
    if(!empty($result)) {
      return array_shift($result);
    }
    return NULL;
  }

  public function storePrediction($competition_id, Prediction $prediction) {
    $prediction->setCompetitionid($competition_id);

    return $this->storeEntity($prediction);
  }

  /**
   *
   * @param int $competition_id
   * @param int $match_id
   * @param int $user_id
   *
   * @return Prediction
   */
  public function findMatchPrediction($competition_id, $match_id, $user_id) {
    $conditions[] = new Condition('competitionid', $competition_id);
    $conditions[] = new Condition('matchid', $match_id);
    $conditions[] = new Condition('userid', $user_id);
    $order = array();

    $result = $this->findEntities('Prediction', $conditions, $order);
    if(!empty($result)) {
      return array_shift($result);
    }
    return NULL;

  }
  /*
   public function findMatchPredictions($competition_id, $match_id) {
  $conditions[] = new Condition('competitionid', $competition_id);
  $conditions[] = new Condition('matchid', $match_id);
  $order = array();

  return $this->findEntities('Prediction', $conditions, $order);

  }
  */

  public function findPredictions($competition_id, $user_id) {
    $conditions[] = new Condition('competitionid', $competition_id);
    $conditions[] = new Condition('userid', $user_id);

    $order = array(new Order('matchid'));

    return $this->findEntities('Prediction', $conditions, $order);

  }
}