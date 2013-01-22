<?php 


function _tippgame_competition_gameday_form(Competition $competition, Gameday $gameday, $viewmode = 'row') {
  $form = array();

  $detailed = (bool)($viewmode=='detail');

  if($gameday) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $gameday->getId(),
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
    '#default_value' => $gameday ? $gameday->getName() : '',
  );
  $form['isallornothing'] = array(
    '#type' => 'checkbox',
    '#title' => t('Knockout Gameday'),
    '#title_display' => $detailed ? 'before' : 'invisible',
    '#default_value' => $gameday ? $gameday->getIsallornothing() : '',
  );
  if(!$detailed) {
    $form['weight'] = array(
      '#type' => 'textfield',
      '#title' => t('Initial group set position'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#size' => 2,
      '#default_value' => $gameday ? $gameday->getWeight() : '',
    );
  } else {
    $form['weight'] = array(
      '#type' => 'hidden',
      '#value' => $gameday ? $gameday->getWeight() : '',
    );
  }

  return $form;
}

/**
 * Implements hook_form().
 */
function tippgame_competition_gamedays_form($form, &$form_state, Competition $competition) {
  $form = array();

  if(!$competition) {
    drupal_set_message(t('No valid competition'), 'error');
    return $form;
  }

  $form['data'] = array(
    '#tree' => TRUE,
    '#theme' => 'tippgame_competition_admin_form_table',
    '#c_render_options' => array(
      'entityname' => 'gameday',
      'colsheader' => array(
        'name' => t('Name'),
        'isallornothing' => t('Knockout Round'),
        'actions' => '',
      ),
      'cols' => array(
        'name',
        'isallornothing',
        'actions',
      ),
      'colweight' => 'weight',
    ),
  );

  $gamedays = tippgame_competition_dao()->findGamedays($competition->getId());
  foreach($gamedays as $gameday) {
    $delta = $gameday->getId();

    $form['data'][$delta] = _tippgame_competition_gameday_form($competition, $gameday);

    $form['data'][$delta]['actions']['edit'] = array(
      '#markup' => l(t('Edit'), 'admin/config/tippgame/competition/'.$competition->getId().'/gameday/'.$gameday->getId().'/edit'),
      '#prefix' => '<div class="action">',
      '#suffix' => '</div>',
    );
    $form['data'][$delta]['actions']['delete'] = array(
      '#markup' => l(t('Delete'), 'admin/config/tippgame/competition/'.$competition->getId().'/gameday/'.$gameday->getId().'/delete'),
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

function tippgame_competition_gamedays_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $form_state['redirect'] = 'admin/config/tippgame/competition/';

  if(isset($values['data'])) {
    foreach($values['data'] as $data) {
      $competition_id = $data['competition'];
      $gameday = EntityHelper::array2object($data, 'Gameday');
      tippgame_competition_dao()->storeGameday($gameday);
    }
    drupal_set_message(t('Gamedays updated'));

    $form_state['redirect'] = 'admin/config/tippgame/competition/'.$competition_id.'/gameday';

  } else {
    drupal_set_message(t('Error while updating'), 'error');
  }

}

/**
 * Implements hook_form().
 */
function tippgame_competition_gameday_edit_form($form, &$form_state, Competition $competition, Gameday $gameday = NULL) {
  $form = array();

  if(!$competition) {
    drupal_set_message(t('No valid competition'), 'error');
    return $form;
  }

  $title = t('Create new Gameday', array(), array('context' => 'tippgame'));
  if($gameday) {
    $title = t('Edit Gameday', array(), array('context' => 'tippgame'));
  }
  drupal_set_title($title);

  $form = _tippgame_competition_gameday_form($competition, $gameday, 'detail');

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'admin/config/tippgame/competition/'.$competition->getId().'/gameday'),
  );

  return $form;
}

function tippgame_competition_gameday_edit_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $form_state['redirect'] = 'admin/config/tippgame/competition/'.$values['competition'].'/gameday';

  $gameday = EntityHelper::array2object($values, 'Gameday');
  tippgame_competition_dao()->storeGameday($gameday);

  drupal_set_message(t('Gameday saved'));
}

/**
 * Implements hook_form().
 *
 * Admin delete gameday.
 */
function tippgame_competition_gameday_delete_form($form, &$form_state, $competition, $gameday) {
  if(is_object($competition) && is_object($gameday)) {
    $form['#competition_id'] = $competition->getId();
    $form['#gameday_id'] = $gameday->getId();

    return confirm_form(
        $form,
        t('Are you sure you want to delete gameday %title?', array('%title' => $gameday->getName())),
        'admin/config/tippgame/competition/'.$competition->getId().'/gameday',
        t('Note: This action cannot be undone.'),
        t('Delete'),
        t('Cancel'),
        'competition_gameday_confirm_delete');
  } else {
    return $form();
  }
}

function tippgame_competition_gameday_delete_form_submit($form, &$form_state) {
  $competition_id = $form['#competition_id'];
  $gameday_id = $form['#gameday_id'];
  $form_state['redirect'] = 'admin/config/tippgame/competition/'.$competition_id.'/gameday';

  if(tippgame_competition_dao()->deleteGameday($gameday_id)) {
    drupal_set_message(t('Gameday deleted'));
  } else {
    drupal_set_message(t('Error occured while deleting gameday'));
  }
}

