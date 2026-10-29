# 14 — Consommateur et pixels intégrés, contrôles indépendants prêts

## Résultat et prochaine étape

Le consommateur utilise désormais l'accès préparé pour target et conserve l'API
par nom pour decoy. Le test pixel et l'exemple sont intégrés. Les trois fichiers
correspondent exactement à la livraison contrôlée, sans modification du moteur.
La revue de sûreté et les propositions de gaps peuvent maintenant être parallèles
sur ce candidat fixe ; la campagne runner et l'audit final restent à produire.

## Vérifications effectuées par le lead

- Auteur arrêté, commit propre et diff limité aux trois chemins autorisés.
- Lecture du diff consommateur, des 294 lignes du test graphique et de l'exemple.
  Oracle fixe, états OpenGL, retrait et espion local examinés ; pas de verdict
  indépendant déduit de cette lecture du coordinateur.
- Archive de 62 artefacts et treize empreintes sources/fixtures/binaires/entrées
  recoupées ; ZIP et CRC vérifiés. Archives copiées avant tout retrait du worktree.
- JUnit normaux 502 CPU + 75 OpenGL, ASan 577 : mêmes noms uniques, zéro
  failure/error/skip. Les cinq commandes convenues auteur ont chacune exit 0.
  Canari ASan healthy 0 / use-after-free 86 ; détection de fuites désactivée.
- RED omission update et contrôle draw omis : chacun un échec attendu distinct ;
  intérieur commun noir détecté malgré l'égalité des images. Ciblés restaurés 3/3.
- Le premier script de comparaison traitait une entrée hash structurée comme une
  chaîne ; correction du champ sha256, aucune divergence source observée et aucun
  test relancé pour cette erreur d'analyse.
- Fusion native après retrait temporaire du seul renommage étranger de l'index ;
  entrées et octets de travail étrangers restaurés exactement, commit sélectif.

Ces résultats sont des exécutions auteur recoupées, pas une campagne finale lead
sur le candidat intégré. Le contrôle discriminant C/L/A, la revue, les gaps,
l'audit et la documentation ne sont pas encore PASS.

## Admission QA préparée sans réparer le harness

Les profils natifs résolus correctness-reviewer, test-engineer et test-reviewer
ont été interrogés. Le generator garde deny-all et ses exceptions minimales,
édition/exécution shell/délégation interdites. Une digestion locale fournit
contrat, chemins et données de preuve ; aucune lecture d'un ancien pool externe
n'est nécessaire. L'admission effective et la restitution doivent encore être
observées, pas déduites du seul nom de profil. Aucun grant ajouté.

Les deux lectures indépendantes n'écrivent pas et n'utilisent pas le GPU : elles
peuvent être parallèles. Le runner est le lead, puis audit fresh hors board avec
contexte neutre. Aucun nouveau GO de jalon ni extension d'infrastructure requise.

## Annexe technique

- H2 source intégré : `c283f806f93167b86ab0278d58714764873022ca`.
- Auteur : `460fe52c2098abc8fc68b7cefed4ee3f3845a491` ; base `4e472517`.
- Producteur consommateur : `task_mvqplny5_000k_ldfdp5`.
- Archive : `out/qa-prepared-float/460fe52-consumer-delivery/` ; manifeste SHA256
  `f506bbda49bbdecaadaefa3778aecdc6a9b21e1d1379851df93089349b89eae0`.
- ZIP frère SHA256 `da8f9a9ee8c02a62e8de4c9e9be74c812aef481c1ad113fb74ad7ca812772155`.
- [Entrées QA locales](../qa-integrated-input.md), [premier RED/GREEN](13-premiers-pixels-red-green-recoupe.md).
- L'outil a encore signalé un retrait du worktree impossible après fusion ; branche
  conservée, aucun retry de merge ni nettoyage forcé. Cela ne bloque pas la revue.
