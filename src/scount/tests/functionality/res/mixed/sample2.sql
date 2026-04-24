-- Second SQL sample

ALTER TABLE colors ADD name TEXT;

UPDATE colors
  SET hex = '#000000'
  WHERE id = 1;

SELECT id, hex, name
  FROM colors
  WHERE hex IS NOT NULL;
