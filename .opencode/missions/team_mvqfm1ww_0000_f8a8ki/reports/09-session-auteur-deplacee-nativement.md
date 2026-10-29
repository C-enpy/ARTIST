# 09 — Session auteur raccordée nativement à son worktree

## Problème constaté et réponse bornée

L'auteur a respecté la barrière : après sa transition native implementation/high,
ses outils indiquaient encore main dans le répertoire canonique. Il a transmis
les preuves relatives et n'a effectué aucun edit, build ou commit. Le lead a
traité ce constat d'affectation sans chantier de réparation d'Ensemble.

L'outil natif exposé opencode.session_move a déplacé cette même session vers le
worktree déjà attribué à la même tâche. L'appel a réussi. Aucun autre agent,
fork, modèle, grant, profil ou système de fichiers source n'a été créé/modifié.

## Vérifications du lead après déplacement

- GET session : location.directory pointe maintenant le worktree assigné,
  agent cpp-engineer et modèle openai/gpt-6.1-sol high conservés.
- .git de destination est un fichier gitdir ; branche dédiée
  ensemble-0e26f086-071c-4db2-ab92-3f30763c256b, pas main.
- HEAD de destination : H0=49005ed2ca1803eb57726b27247da01f31555a7d.
  Contrat/plan QA H0 avaient déjà été vérifiés présents à l'empreinte attendue.
- Canonique ARTIST/test-datas/scripts : toujours inchangé ; renommage stagé
  FOREIGN .grepai conservé. Aucun diff local copié ou fusionné vers le worktree.

Le lead n'en déduit pas une réparation générale du bootstrap du plugin. C'est une
action native de coordination sur l'instance déjà admise. Les copies d'instructions
FOREIGN non commitées ne sont pas transférées implicitement ; le brief et le
contrat/QA H0 continuent de définir les limites actuelles.

## Reprise contrôlée

Le plan RO de l'auteur est conforme au contrat : témoins locaux, publishers privés
Loader/Reader, révocation avant retrait/pointee, float strict, miroir CPU et upload
direct, compteurs mock-only et tests lifetime/GL. Les références Serena vides ont
été correctement complétées par grep ciblé, sans prétention d'exhaustivité LSP.

Le lead a autorisé la reprise de la même tâche via team_message, en demandant
avant édition des contrôles RELATIFS de cwd/.git/HEAD/contrat/QA dans le nouvel
environnement. Ne pas répéter l'ancien accès absolu refusé ; s'il subsiste un
écart effectif, rendre le constat sans écrire sur main. Aucun nouveau GO utilisateur
nécessaire : il s'agit d'une précondition technique résolue, pas de la levée d'une
pause opérateur. Le bail build/GPU reste exclusif à cette tranche.

## Traçabilité et limites

- Tâche task_mvqn5vg6_0003_gblyx2, pool-2,
  session ses_e82bd3877ffe5ny3I9Xdv55nxv, ownership inchangé.
- Destination : C:/Users/djden/.local/share/opencode/worktree/3a086c/ensemble-2dc31e0a-f8a8ki-pool-2.
- Appel natif session_move, puis GET public et git en lecture seule via Context Mode.
- Aucun correctif harness, élargissement de droits, override de modèle, nouvelle
  équipe, édition sur main ou résultat RED/GREEN/GPU nouveau revendiqué.
- Le consommateur n'est toujours pas publié ; il attend l'intégration réelle H1.

## Confirmation ultérieure par les outils de l'auteur

L'auteur a ensuite transmis ses vérifications relatives PASS : cwd du worktree,
.git fichier gitdir, branche attendue, H0, état Git propre, contrat présent et
empreinte QA exacte. Context Mode a été redécouvert disponible. Les deux anciennes
remises failed concernent l'ancien rattachement ou la transition avant cette
requalification ; elles ne sont pas des échecs du nouveau contrôle.

La lecture externe de la politique de scripts avait été refusée. Le lead, qui
l'avait déjà lue, a fourni sa digestion dans le brief sans nouveau grant, copie
globale ou nouvel essai de cette lecture. La reprise reste autorisée dans le même
contrat et la même session. Aucun RED/GREEN n'est encore déduit de ces préconditions.
