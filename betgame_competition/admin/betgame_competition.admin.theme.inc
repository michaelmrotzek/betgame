<?php 

/**
 * Returns HTML table representing a entity.
 *
 * @param $variables
 *   An associative array containing:
 *   - data: array form element 'data' to render.
 *     - data['#c_render_options]: array to configure table rendering
 *         - entityname: name of the entity used to generate unique css id
 *         - cols: name of columns to render (references a child form field with this name)
 *         - colshidden: name of hidden columns not to show, but needed for update of a row
 *         - colsheader: table header title for rendered fields. set column name as key and title as value.
 *             do not set or leave empty to render table without a header
 *         - colweight: name of weight column, if set and form field exists the rows of the table will be draggable to sort
 */
function theme_betgame_competition_admin_form_table($variables) {
  $rows = array();

  $data = $variables['data'];

  if ($data) {
    $entityname = '';
    $cols = array();
    $colshidden = array();
    $colweight = 'weight';
    $colsheader = array();

    extract($data['#c_render_options']);

    $enableTableDrag = FALSE;

    foreach (element_children($data) as $key) {
      $form = &$data[$key];
      $header = array();

      $fields = array();

      $hiddencontent = drupal_render($form['id']) . drupal_render($form['competition']);
      if(!empty($colshidden)) {
        foreach($colshidden as $col) {
          $hiddencontent .= drupal_render($form[$col]);
        }
      }

      $fields[] = array('data' => $hiddencontent);
      $header['_id'] = '';

      if(isset($form[$colweight])) {
        $enableTableDrag = TRUE;

        $form[$colweight]['#attributes']['class'] = array($entityname.'-weight');
        $fields[$colweight] =  array('data' => drupal_render($form[$colweight]));
        $header[$colweight] = isset($colsheader[$colweight]) ? $colsheader[$colweight] : t('Weight');
      }

      foreach($cols as $col) {
        $fields[$col] =  array('data' => drupal_render($form[$col]));
        $header[$col] = isset($colsheader[$col]) ? $colsheader[$col] : '';
      }

      $tablerow = array('data' => $fields);

      if($enableTableDrag) {
        $tablerow['class'] =  array('draggable');
      }

      $rows[] = $tablerow;
    }

    if($enableTableDrag) {
      drupal_add_tabledrag($entityname.'-table', 'order', 'sibling', $entityname.'-weight');
    }

  }

  if(empty($colsheader)) {
    // if no header set, leave it out
    $header = array();
  }

  $build = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => $entityname.'-table'),
  );

  $output = drupal_render($build);
  return $output;
}