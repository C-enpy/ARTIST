# 06 — Démarrage approuvé, première admission QA observée sous OpenAI

## Objectif et étape

La mission peut désormais exécuter la préparation QA de l'accès float adopté.
Le start utilisateur a approuvé la révision1 de la politique présentée. Le lead
poursuit dans le mandat long run, sans demander un nouveau GO à chaque jalon.
Les auteurs restent non publiés tant que la QA, les oracles et le commit H0
ne sont pas acquis.

## Observations natives du lead

- team_status : politique approved v1, mission long_run/running, classification
  manual sans appel ; une seule tâche in_progress et un qa-planner auto-admis.
  Aucun spawn supplémentaire ou modèle forcé par le lead.
- team_view sans navigation : session native nouvelle identifiée.
- GET natif de session : agent qa-planner, modèle gpt-6.1-sol/provider openai,
  variante high ; permissions de session [] (aucun override hérité) et répertoire
  canonique ARTIST, pas de worktree auteur.
- Inspection de contexte pour qualification, pas polling de progression : dix
  événements à l'instant lu, commençant par agent-switched/model-switched/un seul
  user puis assistants. Pas de reprise de l'ancienne session QA conditionnelle.
- Des appels read et skill completed ont été observés, sans lire/valider un
  candidat final pendant ce contrôle.
- GET du profil natif résolu qa-planner pour la location ARTIST : profil subagent,
  règles restrictives présentes (deny général suivi des exceptions lecture,
  recherche/communication ; edit/shell/subagent deny et browser deny).
  Les règles globales et celles du profil sont rendues dans leur ordre natif.

Ces observations établissent le profil réellement nommé, le modèle OpenAI,
l'absence d'override de session et l'activité de lecture de cette admission.
Elles ne certifient ni sandbox complète ni refus dynamique de tout outil interdit.
La réception et la validation du résultat final restent attendues ; ne pas marquer
la tâche complète ou le plan définitivement qualifié avant cette remise.

## Contrôles indépendants pendant la préparation

Le lead a relu l'état Git sans mutation : HEAD2c8e9f2, sources moteur/tests/shaders
et entrées de build inchangées. Un renommage .grepai est déjà stagé et FOREIGN :
il doit être conservé hors du futur commit sélectif de préparation. Les seuls
fichiers possédés sont le dossier de mission ; aucun auteur n'a de worktree.

Calcul géométrique CPU de l'oracle proposé, pas test GPU : viewport32×32,
triangle NDC(-.75,-.75),(.75,-.75),(0,.75), centres de pixels OpenGL bas-gauche.
Pour f2Scalar0 puis.5 :
- pixel(8,8) : intérieur puis fond ; distance minimale aux arêtes >1.56pixel ;
- pixel(24,24) : fond puis intérieur ; distance minimale >4.69pixels ;
- pixel(20,16) : intérieur commun, distance minimale >1.11pixel ;
- pixel(1,1) : fond commun, distance minimale >1.11pixel.
Cela soutient la discriminance mathématique des samples et laisse la preuve
réelle framebuffer/rasterisation à exécuter après le RED et le code de test.
Autres uniforms et états GL doivent être initialisés explicitement.

## Suite autonome, sans nouveau démarrage

Attendre la restitution retenue de cette seule tâche ; la récupérer par la voie
native, contrôler profil/lectures/limites/oracles, puis persister et committer
sélectivement contrat et plan QA avant auteur. La qualification Go429 antérieure
et l'ancienne remise QA ne remplacent aucune de ces étapes.
Un résultat partiel ou refus conserve une barrière bornée ; aucune réadmission
en boucle, permission élargie, autre fournisseur ou réparation du harness.

## Preuves techniques

- Politique approuvée révision1, empreinte native présentée :
  7a33af79375aa8ac430b3a5574608fc4873bbd8810ce151ca771d63450a806d0.
- Première tâche : task_mvqmfwpt_0001_vim66t ; membre pool-1 ; session
  ses_e82cd2b71ffeDxUbgW6PUuLh1C ; parent ses_e8390245bffdS3NfX0qwRMR5ed.
- GET publics via CLI existante/Context Mode : session, contexte (metadata et
  outils seulement, sans reasoning), agent/qa-planner avec location[directory]
  explicite ARTIST. Aucun grant, mutation de session ou transition handoff.
- Calcul barycentrique/distance d'arête JavaScript en Context Mode ; aucun
  programme moteur compilé, rendu exécuté, mesure de temps ou preuve image ici.
- [Contrat](../contract.md), [QA encore conditionnelle](../qa-plan.md),
  [publication échelonnée](../publication-plan.json).
