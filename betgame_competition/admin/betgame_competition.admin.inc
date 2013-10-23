<?php 

/**
 * Implements hook_form().
 *
 * Admin table of all existing competitions.
 */
function betgame_competitions_form($form, &$form_state) {
  $form = array();

  $header = array(
    'id' => array('data' => t('Id', array(), array('context' => 'betgame')), 'field' => 'id'),
    'name' => array('data' => t('Name', array(), array('context' => 'betgame')), 'field' => 'name', 'sort' => 'desc'),
    'matchplan' => t('Matchplan', array(), array('context' => 'betgame')),
    'data' => t('Administrate...', array(), array('context' => 'betgame')),
    'actions' => '',
  );

  $options = array();

  $competitions = betgame_competition_dao()->findCompetitions();
  foreach($competitions as $competition) {
    $matchplan = betgame_matchplan_dao()->findMatchPlan($competition->getMatchplan());

    $teams = count(betgame_competition_dao()->findTeams($competition->getId())) . '/' . $matchplan->getTeams();
    $groups = count(betgame_competition_dao()->findGroups($competition->getId())) . '/' . $matchplan->getGroups();
    $gamedays = count(betgame_competition_dao()->findGamedays($competition->getId())) . '/' . $matchplan->getGamedays();
    $matches = count(betgame_competition_dao()->findMatchs($competition->getId())) . '/' . count(betgame_matchplan_dao()->findFixtures($matchplan->getId()));

    $data = '';
    $data .= '<ul>';
    $data .= '<li>';
    $data .= l(t('Groups', array(), array('context' => 'betgame')), 'admin/config/betgame/competition/'.$competition->getId().'/group') . ' (' . $groups . ')';
    $data .= '</li>';
    $data .= '<li>';
    $data .= l(t('Teams', array(), array('context' => 'betgame')), 'admin/config/betgame/competition/'.$competition->getId().'/team') . ' (' . $teams . ')';
    $data .= '</li>';
    $data .= '<li>';
    $data .= l(t('Gamedays', array(), array('context' => 'betgame')), 'admin/config/betgame/competition/'.$competition->getId().'/gameday') . ' (' . $gamedays . ')';
    $data .= '</li>';
    $data .= '<li>';
    $data .= l(t('Matches', array(), array('context' => 'betgame')), 'admin/config/betgame/competition/'.$competition->getId().'/match') . ' (' . $matches . ')';
    $data .= '</li>';
    $data .= '</ul>';
    
    $actions = array(
      l(t('Edit'), 'admin/config/betgame/competition/'.$competition->getId().'/edit'),
      l(t('Delete'), 'admin/config/betgame/competition/'.$competition->getId().'/delete'),
    );

    $options[$competition->getId()] = array(
      'id' => $competition->getId(),
      'name' => l($competition->getName(), 'betgame/competition/'.$competition->getId()),
      'matchplan' => $matchplan->getName(),
      'data' => $data,
      'actions' => implode('<br/>', $actions)
    );
  }

  $form['data'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No competitions exists', array(), array('context' => 'betgame')),
  );


  return $form;
}

/**
 * Implements hook_form().
 *
 * Admin add/edit of a competition.
 */
function betgame_competition_edit_form($node, &$form_state, Competition $competition = NULL) {
  $form = array();

  $title = t('Create new Competition', array(), array('context' => 'betgame'));
  if($competition) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $competition->getId(),
    );
    $title = t('Edit Competition', array(), array('context' => 'betgame'));
  }
  
  drupal_set_title($title);

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name', array(), array('context' => 'betgame')),
    '#required' => TRUE,
    '#default_value' => $competition ? $competition->getName() : '',
  );

  $options = array();
  $matchplans = betgame_matchplan_dao()->findMatchPlans();
  foreach($matchplans as $m) {
    $options[$m->getId()] = $m->getName();
  }

  $form['matchplan'] = array(
    '#type' => 'select',
    '#title' => t('Matchplan', array(), array('context' => 'betgame')),
    '#options' => $options,
    '#default_value' => $competition ? $competition->getMatchplan() : '',
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function betgame_competition_edit_form_submit($node, &$form_state) {
  $form_state['redirect'] = 'admin/config/betgame/competition';

  $values = $form_state['values'];

  $tx = db_transaction();
  try {
    $competition = EntityHelper::array2object($values, 'Competition');
    $competition_id = betgame_competition_dao()->storeCompetition($competition);

    // auto create groups and teams and gamedays
    betgame_competition_controller()->autoCreateGroupsTeams($competition_id);

    betgame_competition_controller()->generateMatches($competition_id);

  } catch(Exception $e) {
    $tx->rollback();
    throw $e;
  }
}

/**
 * Implements hook_form().
 *
 * Admin delete competition.
 */
function betgame_competition_delete_form($form, &$form_state, $competition = NULL) {
  if(is_object($competition)) {
    $form['#competition_id'] = $competition->getId();


    return confirm_form(
        $form,
        t('Are you sure you want to delete competition %title?', array('%title' => $competition->getName())),
        'admin/config/betgame/competition',
        t('Note: This action cannot be undone.'),
        t('Delete'),
        t('Cancel'),
        'competition_confirm_delete');
  } else {
    return array();
  }
}

function betgame_competition_delete_form_submit($form, &$form_state) {
  $competition_id = $form['#competition_id'];
  $form_state['redirect'] = 'admin/config/betgame/competition';

  if(betgame_competition_dao()->deleteCompetition($competition_id)) {
    drupal_set_message(t('Competition deleted'));
  } else {
    drupal_set_message(t('Error occured while deleting competition'));
  }
}

/**
 * Implements hook_form().
 *
 * Admin generate mactches based on fixtures of matchplan.
 */
function betgame_competition_generatefixtures_form($form, &$form_state, Competition $competition = NULL) {
  $form['#competition_id'] = $competition->getId();

  return confirm_form(
      $form,
      t('Are you sure you want to generate matches based on matchplan fixtures for competition %title?', array('%title' => $competition->getName())),
      'admin/config/betgame/competition/'.$competition->getId().'',
      t('Note: It does not reset kickoff or location data and will not delete any existing matches. This action cannot be undone.'),
      t('Generate'),
      t('Cancel'),
      'competition_confirm_generatefixtures');
}

function betgame_competition_generatefixtures_form_submit($form, &$form_state) {
  $competition_id = $form['#competition_id'];
  $form_state['redirect'] = 'admin/config/betgame/competition/'.$competition_id.'/match';

  betgame_competition_controller()->generateMatches($competition_id);

  drupal_set_message(t('Matches generated'));

}

