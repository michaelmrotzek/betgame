<?php


// /**
//  * Implements hook_form().
//  */
// function betgame_matchplan_edit_form($form, &$form_state, MatchPlan $matchplan = NULL) {
//   $form = array();
  
//   drupal_set_message('QA missing. Don´t know if it´s working', 'warning');

//   $title = t('Create new Match Plan', array(), array('context' => 'betgame'));
//   if($matchplan) {
//     $form['id'] = array(
//       '#type' => 'hidden',
//       '#value' => $matchplan->getId(),
//     );
//     $title = t('Edit Match Plan', array(), array('context' => 'betgame'));
//   }

//   drupal_set_title($title);

//   $form['desc'] = array(
//     '#type' => 'container'
//   );
//   $form['desc']['title'] = array(
//     '#markup' => ''
//   );

//   $form['name'] = array(
//     '#type' => 'textfield',
//     '#title' => t('Name', array(), array('context' => 'betgame')),
//     '#required' => TRUE,
//     '#default_value' => $matchplan ? $matchplan->getName() : '',
//   );


//   $form['section'] = array(
//     '#type' => 'container',
//     '#attributes' => array('class' => array('container-inline')),
//   );
//   $form['section']['teams'] = array(
//     '#type' => 'textfield',
//     '#title' => t('Teams', array(), array('context' => 'betgame')),
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#required' => TRUE,
//     '#default_value' => $matchplan ? $matchplan->getTeams() : 0,
//   );

//   $form['section']['groups'] = array(
//     '#type' => 'textfield',
//     '#title' => t('Groups', array(), array('context' => 'betgame')),
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#required' => TRUE,
//     '#default_value' => $matchplan ? $matchplan->getGroups() : 0,
//   );

//   $form['section']['gamedays'] = array(
//     '#type' => 'textfield',
//     '#title' => t('Gamedays', array(), array('context' => 'betgame')),
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#required' => TRUE,
//     '#default_value' => $matchplan ? $matchplan->getGamedays() : 0,
//   );

//   // dynamic fixtures

//   $form['fixtures-wrapper'] = array(
//     '#tree' => FALSE,
//     '#prefix' => '<div id="fixtures">',
//     '#suffix' => '</div>',
//   );

//   $form['fixtures-wrapper']['fixtures'] = array(
//     '#tree' => TRUE,
//     '#prefix' => '<div id="fixtures-ajax-replace">',
//     '#suffix' => '</div>',
//     '#theme' => 'betgame_matchplan_admin_form_fixtures',
//   );

//   $form['fixtures-wrapper']['addfixture'] = array(
//     '#type' => 'submit',
//     '#submit' => array('betgame_matchplan_add_fixture_submit'),
//     '#value' => t('Add Fixture', array(), array('context' => 'betgame')),
//     '#ajax' => array(
//       'callback' => 'betgame_addfixture_ajax_callback',
//       'wrapper'  => 'fixtures-ajax-replace',
//       //'method' => 'append',
//       'effect' => 'fade',
//     ),
//     '#limit_validation_errors' => array(),
//   );

//   $fixtures = array();
//   if($matchplan && !$form_state['submitted']) {
//     $data = betgame_matchplan_dao()->findFixtures($matchplan->getId());
//     foreach($data as $f) {
//       $fixtures[] = (array) $f;
//     }
//   }

//   $fixtures = array_merge(_betgame_matchplan_extract_fixtures($form_state), $fixtures);

//   if (isset($form_state['num_fixtures']) && $form_state['num_fixtures'] > count($fixtures)) {
//     $fixtures[] = array();
//   }
//   $form_state['num_fixtures'] = count($fixtures);

//   // If the "Remove" button was clicked for a fixture, we need to remove that tab
//   // from the form.
//   if (isset($form_state['to_remove'])) {
//     unset($fixtures[$form_state['to_remove']]);
//     unset($form_state['to_remove']);
//     $form_state['num_fixtures']--;
//   }

//   foreach($fixtures as $delta => $fixture) {
//     $form['fixtures-wrapper']['fixtures'][$delta] = _betgame_matchplan_fixture_form($delta, $fixture);
//     $form['fixtures-wrapper']['fixtures'][$delta]['remove'] = array(
//       '#type' => 'submit',
//       '#submit' => array('betgame_matchplan_remove_fixture_submit'),
//       '#value' => t('Remove', array(), array('context' => 'betgame')),
//       '#limit_validation_errors' => array(),
//     );
//   }

//   $form['actions'] = array('#type' => 'actions');
//   $form['actions']['submit'] = array(
//     '#type' => 'submit',
//     '#value' => t('Save'),
//   );

//   return $form;
// }

// function _betgame_matchplan_fixture_form($delta, array $fixture) {
//   $form['#tree'] = TRUE;


//   $form['id'] = array(
//     '#type' => 'hidden',
//     '#default_value' => isset($fixture['id']) ? $fixture['id'] : NULL,
//   );

//   $form['fixturetype'] = array(
//     '#type' => 'radios',
//     '#options' => array(
//       'FixtureGroupMatch' =>  t('Group match', array(), array('context' => 'betgame')),
//       'FixtureGroupPhaseResult' =>  t('Group phase result', array(), array('context' => 'betgame')),
//       'FixtureAllOrNothingMatch' =>  t('K.O. match result', array(), array('context' => 'betgame')),
//     ),
//     '#default_value' => isset($fixture['fixturetype']) ? $fixture['fixturetype'] : 'FixtureGroupMatch',
//   );

//   $form['home'] = array(
//     '#type' => 'container',
//     //'#attributes' => array('class' => array('container-inline')),
//   );
  
//   $homegroup = array(
//     '#type' => 'textfield',
//     '#title' => t('Group', array(), array('context' => 'betgame')),
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['homegroup']) ? $fixture['homegroup'] : NULL,
//     '#states' => array(
//       'required' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('!value' => 'FixtureAllOrNothingMatch'),
//       ),
//     ),
//   );
//   $homegroupsetposition = array(
//     '#type' => 'textfield',
//     '#title' => t('Set position', array(), array('context' => 'betgame')),
//     //     '#title_display' => 'invisible',
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['homegroupsetposition']) ? $fixture['homegroupsetposition'] : NULL,
//     '#states' => array(
//       'required' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureGroupMatch'),
//       ),
//     ),
//   );
//   $homegroupresultposition = array(
//     '#type' => 'textfield',
//     '#title' => t('Result position', array(), array('context' => 'betgame')),
//     //     '#title_display' => 'invisible',
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['homegroupresultposition']) ? $fixture['homegroupresultposition'] : NULL,
//     '#states' => array(
//       'required' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureGroupPhaseResult'),
//       ),
//     ),
//   );
//   $homegamenumber = array(
//     '#type' => 'textfield',
//     '#title' => t('Game Number', array(), array('context' => 'betgame')),
//     //     '#title_display' => 'invisible',
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['homegamenumber']) ? $fixture['homegamenumber'] : NULL,
//     '#states' => array(
//       'required' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureAllOrNothingMatch'),
//       ),
//     ),
//   );

//   $form['home']['group'] = array(
//     '#type' => 'container',
//     //'#attributes' => array('class' => array('container-inline')),
//     '#states' => array(
//       'visible' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureGroupMatch'),
//       ),
//     ),
//   );
//   $form['home']['group'][] = $homegroup;
//   $form['home']['group'][] = $homegroupsetposition;

//   $form['home']['groupresult'] = array(
//     '#type' => 'container',
//     //'#attributes' => array('class' => array('container-inline')),
//     '#states' => array(
//       'visible' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureGroupPhaseResult'),
//       ),
//     ),
//   );
//   $form['home']['groupresult'][] = $homegroup;
//   $form['home']['groupresult'][] = $homegroupresultposition;

//   $form['home']['allornothing'] = array(
//     '#type' => 'container',
//     //'#attributes' => array('class' => array('container-inline')),
//     '#states' => array(
//       'visible' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureAllOrNothingMatch'),
//       ),
//     ),
//   );
//   $form['home']['allornothing'][] = $homegamenumber;


//   $guestgroup = array(
//     '#type' => 'textfield',
//     '#title' => t('Group', array(), array('context' => 'betgame')),
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['guestgroup']) ? $fixture['guestgroup'] : NULL,
//     '#states' => array(
//       'required' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureGroupPhaseResult'),
//       ),
//     ),
//   );
//   $guestgroupsetposition = array(
//     '#type' => 'textfield',
//     '#title' => t('Set position', array(), array('context' => 'betgame')),
//     //     '#title_display' => 'invisible',
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['guestgroupsetposition']) ? $fixture['guestgroupsetposition'] : NULL,
//     '#states' => array(
//       'required' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureGroupMatch'),
//       ),
//     ),
//   );
//   $guestgroupresultposition = array(
//     '#type' => 'textfield',
//     '#title' => t('Result position', array(), array('context' => 'betgame')),
//     //     '#title_display' => 'invisible',
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['guestgroupresultposition']) ? $fixture['guestgroupresultposition'] : NULL,
//     '#states' => array(
//       'required' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureGroupPhaseResult'),
//       ),
//     ),
//   );
//   $guestgamenumber = array(
//     '#type' => 'textfield',
//     '#title' => t('Game Number', array(), array('context' => 'betgame')),
//     //     '#title_display' => 'invisible',
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['guestgamenumber']) ? $fixture['guestgamenumber'] : NULL,
//     '#states' => array(
//       'required' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureAllOrNothingMatch'),
//       ),
//     ),
//   );

//   $form['guest'] = array(
//     '#type' => 'container',
//     //'#attributes' => array('class' => array('container-inline')),
//   );

//   $form['guest']['group'] = array(
//     '#type' => 'container',
//     //'#attributes' => array('class' => array('container-inline')),
//     '#states' => array(
//       'visible' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureGroupMatch'),
//       ),
//     ),
//   );
//   $form['guest']['group'][] = $guestgroupsetposition;

//   $form['guest']['groupresult'] = array(
//     '#type' => 'container',
//     //'#attributes' => array('class' => array('container-inline')),
//     '#states' => array(
//       'visible' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureGroupPhaseResult'),
//       ),
//     ),
//   );
//   $form['guest']['groupresult'][] = $guestgroup;
//   $form['guest']['groupresult'][] = $guestgroupresultposition;

//   $form['guest']['allornothing'] = array(
//     '#type' => 'container',
//     //'#attributes' => array('class' => array('container-inline')),
//     '#states' => array(
//       'visible' => array(
//         ':input[name="fixtures['.$delta.'][fixturetype]"]' => array('value' => 'FixtureAllOrNothingMatch'),
//       ),
//     ),
//   );
//   $form['guest']['allornothing'][] = $guestgamenumber;

//   $form['weight'] = array(
//     '#type' => 'textfield',
//     '#title' => t('Weight', array(), array('context' => 'betgame')),
//     '#title_display' => 'invisible',
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['weight']) ? $fixture['weight'] : NULL,
//   );
//   $form['gamedayindex'] = array(
//     '#type' => 'textfield',
//     '#title' => t('Game number index?', array(), array('context' => 'betgame')),
//     '#title_display' => 'invisible',
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#default_value' => isset($fixture['gamedayindex']) ? $fixture['gamedayindex'] : NULL,
//   );
//   $form['gamenumber'] = array(
//     '#type' => 'textfield',
//     '#title' => t('Game number', array(), array('context' => 'betgame')),
//     '#title_display' => 'invisible',
//     '#size' => 4,
//     '#maxlength' => 3,
//     '#required' => TRUE,
//     '#default_value' => isset($fixture['gamenumber']) ? $fixture['gamenumber'] : NULL,
//   );

//   return $form;
// }

function _betgame_matchplan_extract_fixtures($form_state) {
  $fixtures = array();
  if(isset($form_state['input']['fixtures']) && !empty($form_state['input']['fixtures'])) {
    foreach($form_state['input']['fixtures'] as $delta => $data) {
      $fixtures[$delta] = $data;
    }
  }
  return $fixtures;
}

// /**
//  * Submit handler for the "Add Fixture" button.
//  */
// function betgame_matchplan_add_fixture_submit($form, &$form_state) {
//   // Increment the number of tabs to be rendered.
//   $form_state['num_fixtures']++;
//   $form_state['rebuild'] = TRUE;
// }

// /**
//  * Submit handler for the "Remove" button.
//  */
// function betgame_matchplan_remove_fixture_submit($form, &$form_state) {
//   // Get the tab delta for the clicked button.
//   $delta = $form_state['clicked_button']['#parents'][1];
//   $form_state['to_remove'] = $delta;
//   $form_state['rebuild'] = TRUE;
// }

// function betgame_matchplan_edit_form_validate($form, &$form_state) {
//   // TODO: validation finishing
  
//   $fixtures = _betgame_matchplan_extract_fixtures($form_state);
//   foreach($fixtures as $data) {
//     // validate per type and defaukt plan data
//     $teams = $form_state['values'][''];
        
//     switch ($data['fixturetype']) {
//       case 'FixtureGroupMatch':
        
//         if($fixtures['homegroupsetposition'] > $teams) {
//            form_set_error('fixtures]['.$data['gamedayindex'].'][homegroupsetposition', 'too big');
//         }
//         if($fixtures['guestgroupsetposition'] > $teams) {
//           form_set_error('fixtures]['.$data['gamedayindex'].'][guestgroupsetposition', 'too big');
//         }
        
//         break;

//       case 'FixtureGroupPhaseResult':
//         if($fixtures['homegroupresultposition'] > $teams) {
//           form_set_error('fixtures]['.$data['gamedayindex'].'][homegroupresultposition', 'too big');
        
//         }
//         if($fixtures['guestgroupresultposition'] > $teams) {
//           form_set_error('fixtures]['.$data['gamedayindex'].'][guestgroupresultposition', 'too big');
        
//         }
        
//         break;
        
//       case 'FixtureAllOrNothingMatch':
//         ;
//         break;

//       default:
//         ;
//         break;
//     }
//   }
// }

// // TODO: does not work yet
// function betgame_matchplan_edit_form_submit($form, &$form_state) {
//   $form_state['redirect'] = 'admin/config/betgame/matchplan';

//   $values = $form_state['values'];

//   $tx = db_transaction();
//   try {
//     $matchplan = EntityHelper::array2object($values, 'MatchPlan');
//     $matchplan_id = betgame_matchplan_dao()->storeMatchPlan($matchplan);

//     $i = 0;
//     $fixtures = _betgame_matchplan_extract_fixtures($form_state);
//     foreach($fixtures as $data) {
//       $i++;
//       if(isset($data['id']) && !$data['id']) {
//         unset($data['id']);
//       }

//       $fixture = EntityHelper::array2object($data, $data['fixturetype']);
//       $fixture->setMatchplan($matchplan_id);
//       if(!$fixture->getGamedayindex()) {
//         $fixture->setGamedayindex($i);
//       }

//       betgame_matchplan_dao()->storeFixture($fixture);
//     }
//   } catch(Exception $e) {
//     $tx->rollback();
//     throw $e;
//   }
// }


// function betgame_addfixture_ajax_callback($form, $form_state) {
//   return $form['fixtures-wrapper']['fixtures'];
// }


function theme_betgame_matchplan_admin_form_fixtures($variables) {
  $rows= array();

  $fixtures = $variables['fixtures'];
  drupal_add_tabledrag('fixtures-table', 'order', 'sibling', 'fixtures-weight');

  if ($fixtures) {
    foreach (element_children($fixtures) as $key) {
      $fixture = &$fixtures[$key];

      $fixture['weight']['#attributes']['class'] = array('fixtures-weight');

      $fields = array(
        array('data' => drupal_render($fixture['id'])),
        array('data' => drupal_render($fixture['weight'])),
        array('data' => drupal_render($fixture['gamedayindex'])),
        array('data' => drupal_render($fixture['gamenumber'])),
        array('data' => drupal_render($fixture['fixturetype'])),
        array('data' => drupal_render($fixture['home'])),
        array('data' => drupal_render($fixture['guest'])),
        array('data' => drupal_render($fixture['remove'])),
      );

      $tablerow = array(
        'data' => $fields,
        'class' => array('draggable')
      );

      $rows[] = $tablerow;
    }

  }

  $header = array(
    '',
    t('Weight'),
    t('Gameday'),
    t('Game Number'),
    t('Type'),
    t('Home'),
    t('Guest'),
    '',
  );

  $build = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'fixtures-table'),
  );

  $output = drupal_render($build);
  return $output;
}
