<?php 


function _betgame_competition_team_form(Competition $competition, Team $team, $viewmode = 'row') {
  $form = array();

  $detailed = (bool)($viewmode=='detail');

  if($team) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $team->getId(),
    );
  }
  $form['competition'] = array(
    '#type' => 'hidden',
    '#value' => $competition->getId(),
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#title_display' => $detailed ? 'before' : 'invisible',
    '#size' => $detailed ? 50 : 25,
    '#default_value' => $team ? $team->getName() : '',
  );
  $form['shortname'] = array(
    '#type' => 'textfield',
    '#title' => t('Short Name'),
    '#title_display' => $detailed ? 'before' : 'invisible',
    '#size' => $detailed ? 50 : 25,
    '#default_value' => $team ? $team->getShortname() : '',
  );
  if(!$detailed) {
    $form['initialsetposition'] = array(
      '#type' => 'textfield',
      '#title' => t('Initial group set position'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#size' => 2,
      '#default_value' => $team ? $team->getInitialsetposition() : '',
    );
  } else {
    $form['initialsetposition'] = array(
      '#type' => 'hidden',
      '#value' => $team ? $team->getInitialsetposition() : '',
    );
  }

  // TODO: team logo/flag

  return $form;
}


/**
 * Implements hook_form().
 */
function betgame_competition_teams_form($form, &$form_state, Competition $competition) {
  $form = array();

  if(!$competition) {
    drupal_set_message(t('No valid competition'), 'error');
    return $form;
  }


  $form['competition'] = array(
    '#type' => 'hidden',
    '#value' => $competition->getId(),
  );

  $form['data'] = array(
    '#tree' => TRUE,
    '#theme' => 'betgame_competition_admin_form_table',
    '#c_render_options' => array(
      'entityname' => 'team',
      'colsheader' => array(
        'name' => t('Name'),
        'shortname' => t('Short Name'),
        'actions' => '',
      ),
      'cols' => array(
        'name',
        'shortname',
        'actions',
      ),
      'colweight' => 'initialsetposition',
    ),
  );

  $teams = betgame_competition_dao()->findTeams($competition->getId());
  foreach($teams as $team) {
    $delta = $team->getId();

    $form['data'][$delta] = _betgame_competition_team_form($competition, $team);
     
    $form['data'][$delta]['actions']['edit'] = array(
      '#markup' => l(t('Edit'), 'admin/config/betgame/competition/'.$competition->getId().'/team/'.$team->getId().'/edit'),
      '#prefix' => '<div class="action">',
      '#suffix' => '</div>',
    );
    $form['data'][$delta]['actions']['delete'] = array(
      '#markup' => l(t('Delete'), 'admin/config/betgame/competition/'.$competition->getId().'/team/'.$team->getId().'/delete'),
      '#prefix' => '<div class="action">',
      '#suffix' => '</div>',
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  return $form;
}

function betgame_competition_teams_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $form_state['redirect'] = 'admin/config/betgame/competition';

  if(isset($values['data'])) {
    foreach($values['data'] as $data) {
      $competition_id = $data['competition'];
      $team = EntityHelper::array2object($data, 'Team');
      betgame_competition_dao()->storeTeam($team);
    }
    drupal_set_message(t('Teams updated'));
    $form_state['redirect'] = 'admin/config/betgame/competition/'.$competition_id.'/team';

  } else {
    drupal_set_message(t('Error while updating'), 'error');
  }

}

/**
 * Implements hook_form().
 */
function betgame_competition_team_edit_form($form, &$form_state, Competition $competition, Team $team = NULL) {
  $form = array();

  if(!$competition) {
    drupal_set_message(t('No valid competition'), 'error');
    return $form;
  }

  $title = t('Create new Team', array(), array('context' => 'betgame'));
  if($team) {
    $title = t('Edit Team', array(), array('context' => 'betgame'));
  }

  drupal_set_title($title);

  $form = _betgame_competition_team_form($competition, $team, 'detail');

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'admin/config/betgame/competition/'.$competition->getId().'/team'),
  );

  return $form;
}

function betgame_competition_team_edit_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $form_state['redirect'] = 'admin/config/betgame/competition/'.$values['competition'].'/team';

  $team = EntityHelper::array2object($values, 'Team');
  betgame_competition_dao()->storeTeam($team);

  drupal_set_message(t('Team saved'));

}

/**
 * Implements hook_form().
 *
 * Admin delete team.
 */
function betgame_competition_team_delete_form($form, &$form_state, $competition, $team) {
  if(is_object($competition) && is_object($team)) {

    $form['#competition_id'] = $competition->getId();
    $form['#team_id'] = $team->getId();

    return confirm_form(
        $form,
        t('Are you sure you want to delete team %title?', array('%title' => $team->getName())),
        'admin/config/betgame/competition/'.$competition->getId().'/team',
        t('Note: This action cannot be undone.'),
        t('Delete'),
        t('Cancel'),
        'competition_team_confirm_delete');
  } else {
    return array();
  }
}

function betgame_competition_team_delete_form_submit($form, &$form_state) {
  $competition_id = $form['#competition_id'];
  $team_id = $form['#team_id'];
  $form_state['redirect'] = 'admin/config/betgame/competition/'.$competition_id.'/team';

  if(betgame_competition_dao()->deleteTeam($team_id)) {
    drupal_set_message(t('Team deleted'));
  } else {
    drupal_set_message(t('Error occured while deleting team'));
  }
}
