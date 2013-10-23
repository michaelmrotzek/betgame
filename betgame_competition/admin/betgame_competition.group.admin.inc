<?php 


function _betgame_competition_group_form(Competition $competition, Group $group, $viewmode = 'row') {
  $form = array();

  $detailed = (bool)($viewmode=='detail');

  if($group) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $group->getId(),
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
    '#default_value' => $group ? $group->getName() : '',
  );
  $form['shortname'] = array(
    '#type' => 'textfield',
    '#title' => t('Short Name'),
    '#title_display' => $detailed ? 'before' : 'invisible',
    '#size' => $detailed ? 50 : 25,
    '#default_value' => $group ? $group->getShortname() : '',
  );
  if(!$detailed) {
    $form['weight'] = array(
      '#type' => 'textfield',
      '#title' => t('Initial group set position'),
      '#title_display' => $detailed ? 'before' : 'invisible',
      '#size' => 2,
      '#default_value' => $group ? $group->getWeight() : '',
    );
  } else {
    $form['weight'] = array(
      '#type' => 'hidden',
      '#value' => $group ? $group->getWeight() : '',
    );
  }

  return $form;
}

/**
 * Implements hook_form().
 */
function betgame_competition_groups_form($form, &$form_state, Competition $competition) {
  $form = array();

  if(!$competition) {
    drupal_set_message(t('No valid competition'), 'error');
    return $form;
  }

  $form['data'] = array(
    '#tree' => TRUE,
    '#theme' => 'betgame_competition_admin_form_table',
    '#c_render_options' => array(
      'entityname' => 'group',
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
      'colweight' => 'weight',
    ),
  );

  $groups = betgame_competition_dao()->findGroups($competition->getId());
  foreach($groups as $group) {
    $delta = $group->getId();

    $form['data'][$delta] = _betgame_competition_group_form($competition, $group);

    $form['data'][$delta]['actions']['edit'] = array(
      '#markup' => l(t('Edit'), 'admin/config/betgame/competition/'.$competition->getId().'/group/'.$group->getId().'/edit'),
      '#prefix' => '<div class="action">',
      '#suffix' => '</div>',
    );
    $form['data'][$delta]['actions']['delete'] = array(
      '#markup' => l(t('Delete'), 'admin/config/betgame/competition/'.$competition->getId().'/group/'.$group->getId().'/delete'),
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

function betgame_competition_groups_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $form_state['redirect'] = 'admin/config/betgame/competition/';

  if(isset($values['data'])) {
    foreach($values['data'] as $data) {
      $competition_id = $data['competition'];
      $group = EntityHelper::array2object($data, 'Group');
      betgame_competition_dao()->storeGroup($group);
    }
    drupal_set_message(t('Groups updated'));

    $form_state['redirect'] = 'admin/config/betgame/competition/'.$competition_id.'/group';

  } else {
    drupal_set_message(t('Error while updating'), 'error');
  }

}

/**
 * Implements hook_form().
 */
function betgame_competition_group_edit_form($form, &$form_state, Competition $competition, Group $group = NULL) {
  $form = array();

  if(!$competition) {
    drupal_set_message(t('No valid competition'), 'error');
    return $form;
  }

  $title = t('Create new Group', array(), array('context' => 'betgame'));
  if($group) {
    $title = t('Edit Group', array(), array('context' => 'betgame'));
  }
  drupal_set_title($title);

  $form = _betgame_competition_group_form($competition, $group, 'detail');

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'admin/config/betgame/competition/'.$competition->getId().'/group'),
  );

  return $form;
}

function betgame_competition_group_edit_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $form_state['redirect'] = 'admin/config/betgame/competition/'.$values['competition'].'/group';

  $group = EntityHelper::array2object($values, 'Group');
  betgame_competition_dao()->storeGroup($group);

  drupal_set_message(t('Group saved'));
}

/**
 * Implements hook_form().
 *
 * Admin delete group.
 */
function betgame_competition_group_delete_form($form, &$form_state, $competition, $group) {
  if(is_object($competition) && is_object($group)) {
    $form['#competition_id'] = $competition->getId();
    $form['#group_id'] = $group->getId();

    return confirm_form(
        $form,
        t('Are you sure you want to delete group %title?', array('%title' => $group->getName())),
        'admin/config/betgame/competition/'.$competition->getId().'/group',
        t('Note: This action cannot be undone.'),
        t('Delete'),
        t('Cancel'),
        'competition_group_confirm_delete');
  } else {
    return $form();
  }
}

function betgame_competition_group_delete_form_submit($form, &$form_state) {
  $competition_id = $form['#competition_id'];
  $group_id = $form['#group_id'];
  $form_state['redirect'] = 'admin/config/betgame/competition/'.$competition_id.'/group';

  if(betgame_competition_dao()->deleteGroup($group_id)) {
    drupal_set_message(t('Group deleted'));
  } else {
    drupal_set_message(t('Error occured while deleting group'));
  }
}

