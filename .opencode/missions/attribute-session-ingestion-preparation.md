# Préparation — ingestion et rétrospective Attribute-GL

Statut courant : mission d'ingestion approuvée nativement v1, hash
e29794644ac1b57495684a9abe8fa08b58cb27a40d33086822815aa6a354da4e.
Après demande utilisateur de proportionner le travail puis « go », le graphe
initial ci-dessous est conservé comme historique, pas comme une file à exécuter.
Périmètre réduit : une note de session par le lead, contrôles documentaires et
garde-fous au stade proposition ; aucune modification d'agent/skill/permission.
Note : `docs/references/session-2026-10-18-attribute-gl.md`.
Sources et checklist : `.opencode/missions/team_mve3tz7l_000i_xtayy5/`.
Cette ingestion ne clôture pas automatiquement la mission moteur Attribute-GL.

## Objectif et preuves

Ingérer dans le vault ARTIST la session Attribute-GL et ses sous-agents accessibles,
documenter résultats, incidents et rectifications, puis ajouter uniquement les
garde-fous locaux nécessaires et non déjà couverts. Pas de changement moteur,
permissions, routage, modèle global, index d'autres collections ou plugin exécutable.
Le résultat doit distinguer source, interprétation, exécution et mesure.

Plan PM reçu : project-manager, session ses_eaff5e745ffeHyJqmDacfW0CY0,
lecture seule, aucune sous-délégation ni édition. Le PM n'était pas membre de
l'équipe native ; sa tentative de transport a été refusée, restitution finale
native disponible. Pas d'élargissement des permissions pour ce transport.
Méthode PM/LLM, pas JEV. Scores proposés ci-dessous validés par le lead.
Développement éventuel de script/harness hors périmètre : nouvelle tranche>=70
et TDD, pas renommée documentation pour éviter ce plancher.

## Acquisition préparatoire vérifiée

HEAD f7bb171e1c8ca10929467a0fcd50469dbfa69837. Dirty externe harness/profils/
AGENTS/docs/.grepai et deux intentions d'ajout conservés ; aucun reset/stash/
commit global. Les futures écritures évitent ces fichiers par défaut.

Racine native :ses_eb1605bf2ffeAdDg1L6ZxSRQ90.
Borne historique :message utilisateur msg_150091c600010Tw3DVy0Rhrz5E inclus
(demande d'ingestion). Exclure la collecte/rétrospective elle-même des faits de
la mission précédente, pour éviter l'ingestion récursive sans fin.
Inventaire descendants paginé :11pools+1revue fresh hors board ; pas de descendant
supplémentaire historique retourné. Le nouveau PM d'ingestion est séparé.
Corpus historique :13sessions,850entrées natives à la borne (416lead+434enfants).
Ces entrées comprennent aussi idle/synthetic/compaction, pas850messages humains.

API native V2 découverte depuis OpenAPI :
GET /api/session?parentID=...&limit=100, pagination jusqu'à page vide ;
GET /api/experimental/session/{id}/export?sanitize=... ;
GET /api/session/{id}/message?limit=100&order=asc puis cursor, sans order sur suite.
CLI absent du PATH, binaire du processus actif trouvé sans installation/service
restart :AppData/Roaming/ai.opencode.desktop/cli/2.0.32-x64/opencode-cli.exe.
Appels via sa commande api, même découverte/auth native, aucun accès direct DB.

Exports sanitize=true :textes remplacés par des marqueurs, insuffisants pour
distiller le contenu. Exports sanitize=false conservés PRIVÉS sous
out/session-ingestion-2026-10-18/private-raw/. Ensemble14exports avec nouveauPM,
10,100,108octets au snapshot. Taille/sha256/comptes enregistrés par session.
La liste paginée retrouve chaque ID exporté :0ID manquant. Lead et PM évoluaient
pendant la collecte :messages postérieurs aux exports identifiés, non prétendus
inclus dans leur snapshot. Les sessions historiques de workers sont stables.

Artifacts préparatoires :inventory.json,coverage.json,private-coverage.json,
pagination-audit.json et analysis-events.jsonl sous out/session-ingestion-2026-10-18/.
Vue d'analyse exclut les raisonnements internes, messages système et données
provider ; les exports privés ne doivent pas être publiés ou indexés.
git check-ignore confirme out/ ignoré,git ls-files confirme raw non suivi ;
.wiki.json limite artist-wiki à docs/**/*.md,donc raw hors Git ET hors QMD.
Ces vérifications ne sont pas une preuve automatique d'assainissement du contenu.

Limite :exports de l'historique PROJETÉ accessible, pas journal interne complet
du service. Compactions/troncatures et texte redacted doivent être inventoriés.
Le checkpoint seul ne sera jamais présenté comme un verbatim complet.

## Ownership et confidentialité par défaut

- Raw privé immuable, pas d'export brut dans les notes versionnées ; pas de
  secrets, chaînes de raisonnement ou prompts système copiés au vault.
- Autorisation de nouveaux chemins de notes et de skills LOCAUX propres seulement.
  Pas de mutation globale. Tout hunk sur fichier déjà dirty attend autorisation
  précise ou reste proposition non appliquée ; ne pas reprendre le dirty entier.
- Préférer une note de référence complète et des ajouts courts au point canonique.
  Ne pas reproduire les leçons dans chaque agent ou créer une collection de templates.
- Les modèles/providers restent gérés par la politique native présentée/approuvée,
  en conservant les préférences utilisateur antérieures si confirmées dans config.

## Graphe et évaluations PM/LLM

P0 QA préparatoire :qa-planner,C54/R76,RO,fresh. Plan texte puis persistance lead
avant auteurs. P1 collecte lead,C78/R88,enquête sans nouveau script,poursuit la
réconciliation/confidentialité ; acquisition ci-dessus préparatoire déjà disponible.
P0||P1 :aucun état source mutable partagé.

Barrière B1 :périmètre natif approuvé,planQA commité,corpus dérivé assaini/hashé,
sources réellement disponibles aux consommateurs,identitéHEAD. Raw ignoré et
diff noncommité ne sont pas automatiquement transmis aux worktrees.

P2 technique :gpu-engineer ou correctness-reviewer,C65/R59,RO,fresh.
P3 orchestration :correctness-reviewer,C62/R70,RO,fresh.
P2||P3 sur les mêmes sources intégrées :rapports attribués,aucun build/GPU.
Chaque enseignement cite sourceID/messageID,fait,contre-exemple et règle existante/gap.

Barrière B2 :rapports persistés/relus,contradictions conservées,ownership exact
des chemins propres/auteurs décidé ; consommateurs non publiés jusque-là.

P4 notes Obsidian,C63/R62 :nouvelles notes references/session-2026-10-18-attribute-gl*,
manifestes assainis sousdocs/_meta ; index/log seulement hunks explicitement possédés.
P5 garde-fous locaux,C74/R80 :fichiers propres autorisés et références canoniques,
pas AGENTS/orchestrator/QA/harness déjà dirty sans autorisation additionnelle.
P4||P5 :écritures disjointes,un propriétaire par fichier.

P6 intégration lead,C60/R78 :diff allowlist,hashes,frontmatter/liens,absence données
sensibles,permissions/routage inchangés. Pas de nouveaux scripts de contrôle.
P7 gaps QA,test-engineer,C50/R65,RO :contrôles documentaires proposés texte,
pas code exécutable ; si un test/harness est proposé,réevaluer>=70 avant publication.
P8 test-reviewer,C61/R76,fresh HORS board :sources↔couverture↔notes↔garde-fous,
correctifs propriétaires puis intégration/ré-audit. Aucun runner QA autonome.
P9 lead réindex,C38/R48 :un indexeur,artist-wiki seulement,update-wiki.mjs puis
qmd embed -c artist-wiki ; query(intent explicite)/get vérifient la retrouvabilité.

## Hypothèses de leçons à éprouver, pas encore des règles adoptées

- Confusion entre verdict driver et obligation du moteur ; décisionuserQ3 corrige.
- Mauvais oracle GLSL330 arrays ; vérifier spec avant auteur,docs corrigées ensemble.
- Entiers convertis float32 ne prouvent pas consommation exacte ; TF typé/discriminants.
- Statut board pris pour activité réelle ; fin auteur/candidat/intégration distincts.
- Transport QA incompatible et admissionD redondante :limites plugin,ne pas inventer
  qu'un rappel de prompt ou template prouve un correctif d'enforcement.
- CheminsMSVC longs :préflight sortie,expérience /Fo seulement avant restructurer.
- Échappements ContextMode répétés :scripts simples,éviter couchesregex non nécessaires,
  nouvelle hypothèse/expérience plutôt que répétition sans gain d'information.
- Autostash/ITA :ownership et consentement ciblé,sauvegarde/hash/restauration index,
  pas git reset global ni commit forcé d'un état externe.
- Découverte GoogleTest timeout5s :distinguer environnement/RED ; sonde directe a
  discriminé avant unique relance,inutile d'augmenter aveuglément les délais.
- JUnit est formatXML de rapport pourGoogleTest/CTest,pas ajoutJava/JVM.
- Photographie documentaire et statut de clôture doivent refléter ce qui est
  réellement intégré/vérifié,pas les seuls résultats d'un worker.

## Sortie

Registre de couverture borné et vérifié,notes attribuées,incidents+rectifications,
garde-fous nondupliqués avec contre-exemples,contrôles exécutés par lead et audit
fresh sans finding bloquant. Limites d'exhaustivité/confidentialité explicites.
Aucun gainperformance ou correction moteur,aucune conformitédriver inférée.
