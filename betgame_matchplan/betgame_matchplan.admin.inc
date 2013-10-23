<?php

function betgame_matchplan_page() {
  $ra = array(
  );

  $ra['intro'] = array(
    '#markup' => ''
  );


  $ra['list'] = drupal_get_form('betgame_matchplan_form');

  return $ra;
}

/**
 * Implements hook_form().
 */
function betgame_matchplan_form($form, &$form_state) {
  $form = array();

  $header = array(
    'id' => array('data' => t('Id', array(), array('context' => 'betgame')), 'field' => 'id'),
    'name' => array('data' => t('Name', array(), array('context' => 'betgame')), 'field' => 'name', 'sort' => 'desc'),
    'groups' => array('data' => t('Groups', array(), array('context' => 'betgame')), 'field' => 'groups'),
    'teams' => array('data' => t('Teams', array(), array('context' => 'betgame')), 'field' => 'teams'),
    'gamedays' => array('data' => t('Gamedays', array(), array('context' => 'betgame')), 'field' => 'gamedays'),
    'actions' => '',
  );

  $options = array();

  $plans = betgame_matchplan_dao()->findMatchPlans();
  foreach($plans as $matchplan) {
    $options[$matchplan->getId()] = array(
      'id' => $matchplan->getId(),
      'name' => $matchplan->getName(),
      'groups' => $matchplan->getGroups(),
      'teams' => $matchplan->getTeams(),
      'gamedays' => $matchplan->getGamedays(),
      'actions' => ''
//          l(t('Edit'), 'admin/config/betgame/matchplan/'.$matchplan->getId().'/edit')
    );
  }

  $form['data'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No matchplan exists', array(), array('context' => 'betgame')),
  );


  return $form;
}
