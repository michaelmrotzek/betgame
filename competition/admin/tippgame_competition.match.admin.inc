<?php 

function _tippgame_competition_match_form(Competition $competition, Match $match, $viewmode = 'row') {
  $form = array();

  $detailed = (bool)($viewmode=='detail');

  if($match) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $match->getId(),
    );
  }
  $form['competition'] = array(
    '#type' => 'hidden',
    '#value' => $competition->getId(),
  );

  $form['type'] = array(
    '#type' => 'hidden',
    '#value' => $match->getType(),
  );
  $form['kickoff'] = array(
    '#type' => 'textfield',
    '#title' => t('Kickoff'),
    '#title_display' => $detailed ? 'before' : 'invisible',
    '#size' => $detailed ? 50 : 25,
    '#default_value' => $match ? $match->getKickoff() : 0,
  );
  $form['location'] = array(
    '#type' => 'textfield',
    '#title' => t('Location'),
    '#title_display' => $detailed ? 'before' : 'invisible',
    '#size' => $detailed ? 50 : 25,
    '#default_value' => $match ? $match->getLocation() : '',
  );
  $form['gamenumber'] = array(
    '#type' => 'textfield',
    '#disabled' => $match ? TRUE : FALSE,
    '#title' => t('Game Number'),
    '#title_display' => $detailed ? 'before' : 'invisible',
    '#default_value' => $match ? $match->getGamenumber() : '',
    '#size' => 2,
  );
  if($detailed) {
    $form['gameday'] = array(
      '#type' => 'select',
      '#disabled' => TRUE,
      '#title' => t('Gameday'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#default_value' => $match ? $match->getGameday() : 0,
      '#options' => _tippgame_competition_select_gameday($competition),
    );
    $form['groupid'] = array(
      '#type' => 'select',
      '#disabled' => TRUE,
      '#title' => t('Group'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#default_value' => $match ? $match->getGroupid() : 0,
      '#options' => _tippgame_competition_select_group($competition),
    );
  } else {
    $form['gameday'] = array(
      '#type' => 'hidden',
      '#value' => $match ? $match->getGameday() : 0,
    );
    $form['groupid'] = array(
      '#type' => 'hidden',
      '#value' => $match ? $match->getGroupid() : 0,
    );
  }

  if(!$match->isPlaceholder()) {
    $form['hometeam'] = array(
      '#type' => 'select',
      '#title' => t('Home'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#default_value' => $match ? $match->getHometeam() : '',
      '#options' => _tippgame_competition_select_team($competition),
    );
    $form['guestteam'] = array(
      '#type' => 'select',
      '#title' => t('Guest'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#default_value' => $match ? $match->getGuestteam() : '',
      '#options' => _tippgame_competition_select_team($competition),
    );
  } else {
    $form['hometeam'] = array(
      '#type' => 'hidden',
      '#title' => t('Home'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#value' => $match ? $match->getHometeam() : '',
      '#suffix' => theme('tippgame_competition_match_team', array('match' => $match, 'type' => 'home', 'useShortname' => !$detailed)),
    );
    $form['guestteam'] = array(
      '#type' => 'hidden',
      '#title' => t('Guest'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#value' => $match ? $match->getGuestteam() : '',
      '#suffix' => theme('tippgame_competition_match_team', array('match' => $match, 'type' => 'guest', 'useShortname' => !$detailed)),
    );
  }

  $form['placeholderhomeval'] = array(
    '#type' => 'hidden',
    '#value' => $match ? $match->getPlaceholderhomeval() : '',
  );
  $form['placeholderguestval'] = array(
    '#type' => 'hidden',
    '#value' => $match ? $match->getPlaceholderguestval() : '',
  );

  if($match->getType() == MATCH_TYPE_ALLORNOTHING_FIRSTLEG) {
    $form['returnmatchgamenumber'] = array(
      '#type' => 'select',
      '#title' => t('Return Match'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#default_value' => $match ? $match->getReturnmatchgamenumber() : '',
      '#options' => _tippgame_competition_select_match($competition),
    );
  }

  if(!$detailed) {
    $form['_result'] = array(
      '#markup' => theme('tippgame_competition_match_result', array('match' => $match)),
    );
  }

  return $form;
}

/**
 * Implements hook_form().
 */
function tippgame_competition_matchs_form($form, &$form_state, Competition $competition) {
  $form = array();

  if(!$competition) {
    drupal_set_message(t('No valid competition'), 'error');
    return $form;
  }

  $form['data'] = array(
    '#tree' => TRUE,
    '#theme' => 'tippgame_competition_admin_form_table',
    '#c_render_options' => array(
      'entityname' => 'match',
      'colsheader' => array(
        'kickoff' => t('Kickoff'),
        'location' => t('Location'),
        'gamenumber' => t('Game Number'),
        'hometeam' => t('Home'),
        'guestteam' => t('Guest'),
        '_result' => t('Result'),
        'returnmatchgamenumber' => t('Return leg'),
        'actions' => '',
      ),
      'cols' => array(
        'kickoff',
        'location',
        'gamenumber',
        'hometeam',
        'guestteam',
        '_result',
        'returnmatchgamenumber',
        'actions',
      ),
      'colshidden' => array(
        'type',
        'gameday',
        'groupid',
        'placeholderhomeval',
        'placeholderguestval,'
      ),
      'colweight' => NULL,
    ),
  );

  $matchs = tippgame_competition_dao()->findMatchs($competition->getId());
  foreach($matchs as $match) {
    $delta = $match->getId();

    $form['data'][$delta] = _tippgame_competition_match_form($competition, $match);

    //$form['data'][$delta]['actions']['edit'] = array(
    //  '#markup' => l(t('Edit'), 'admin/config/tippgame/competition/'.$competition->getId().'/match/'.$match->getId().'/edit'),
    //  '#prefix' => '<div class="action">',
    //  '#suffix' => '</div>',
    //);
    //$form['data'][$delta]['actions']['delete'] = array(
    //  '#markup' => l(t('Delete'), 'admin/config/tippgame/competition/'.$competition->getId().'/match/'.$match->getId().'/delete'),
    //  '#prefix' => '<div class="action">',
    //  '#suffix' => '</div>',
    //);
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  return $form;
}

function tippgame_competition_matchs_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $form_state['redirect'] = 'admin/config/tippgame/competition';

  if(isset($values['data'])) {
    foreach($values['data'] as $data) {
      $competition_id = $data['competition'];
      $match = EntityHelper::array2object($data, 'Match');
      tippgame_competition_dao()->storeMatch($match);

      // we have to set return matches marked so that we can handle it
      if($data['returnmatchgamenumber']) {
        if(($m = tippgame_competition_dao()->findMatchByGamenumber($competition_id, $data['returnmatchgamenumber']))) {
          $m->setType(MATCH_TYPE_ALLORNOTHING_RETURNLEG);
          $m->setReturnmatchgamenumber($match->getGamenumber());

          tippgame_competition_dao()->storeMatch($m);
        }
      }
    }
    drupal_set_message(t('Matches updated'));

    $form_state['redirect'] = 'admin/config/tippgame/competition/'.$competition_id.'/match';

  } else {
    drupal_set_message(t('Error while updating'), 'error');
  }


}

/**
 * Implements hook_form().
 */
function tippgame_competition_match_edit_form($form, &$form_state, Competition $competition, Match $match = NULL) {
  $form = array();

  if(!$competition) {
    drupal_set_message(t('No valid competition'), 'error');
    return $form;
  }

  $title = t('Create new Match', array(), array('context' => 'tippgame'));
  if($match) {
    $title = t('Edit Match', array(), array('context' => 'tippgame'));
  }

  drupal_set_title($title);

  $form += _tippgame_competition_match_form($competition, $match, 'detail');

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'admin/config/tippgame/competition/'.$competition->getId().'/match'),
  );

  return $form;
}

function tippgame_competition_match_edit_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $form_state['redirect'] = 'admin/config/tippgame/competition/'.$values['competition'].'/match';

  $match = EntityHelper::array2object($values, 'Match');
  tippgame_competition_dao()->storeMatch($match);

  // we have to set return matches marked so that we can handle it
  if($data['returnmatchgamenumber']) {
    if(($m = tippgame_competition_dao()->findMatchByGamenumber($competition_id, $data['returnmatchgamenumber']))) {
      $m->setType(MATCH_TYPE_ALLORNOTHING_RETURNLEG);
      $m->setReturnmatchgamenumber($match->getGamenumber());

      tippgame_competition_dao()->storeMatch($m);
    }
  }

  drupal_set_message(t('Match saved'));
}

/**
 * Implements hook_form().
 *
 * Admin delete match.
 */
function tippgame_competition_match_delete_form($form, &$form_state, $competition, $match) {
  if(is_object($competition) && is_object($match)) {

    $form['#competition_id'] = $competition->getId();
    $form['#match_id'] = $match->getId();

    return confirm_form(
        $form,
        t('Are you sure you want to delete match %title?', array('%title' => $match->getGamenumber())),
        'admin/config/tippgame/competition/'.$competition->getId().'/match',
        t('Note: This action cannot be undone.'),
        t('Delete'),
        t('Cancel'),
        'competition_match_confirm_delete');
  } else {
    return array();
  }
}

function tippgame_competition_match_delete_form_submit($form, &$form_state) {
  $competition_id = $form['#competition_id'];
  $match_id = $form['#match_id'];
  $form_state['redirect'] = 'admin/config/tippgame/competition/'.$competition_id.'/match';

  if(tippgame_competition_dao()->deleteMatch($match_id)) {
    drupal_set_message(t('Match deleted'));
  } else {
    drupal_set_message(t('Error occured while deleting match'));
  }
}

