# 02 — Correction locale du catalogue

Date : 2026-10-24. Étape : configuration, toujours suspendue et non approuvée.

## Changement depuis le rapport 01

Correction utilisateur : privilégier les connexions OpenAI Sol/Luna, OpenCode Go et Z AI GLM Flash. Seul le brouillon de cette mission a été modifié, en plus du présent rapport ; aucune configuration globale, source, tâche ou permission modifiée.

Dans [model-policy.json](../model-policy.json) :
- ordre des providers : `openai`, `opencode-go`, `zai-coding-plan`, `opencode`, `cursor` ;
- les entrées existantes `openai/gpt-6.1-sol#high` et `openai/gpt-6-luna#high` sont remontées en tête ;
- les 29 couples, métriques, disponibilités et variantes sont conservés, sans exclusion implicite ;
- seuil 50, classification et `verification: { "level": "proportionate" }` inchangés. Aucune correction QA ou classification demandée, donc aucun nouveau champ.

## Vérifié et limites

Découverte native `opencode.models` : OpenAI expose `gpt-6.1-sol`, variantes low/medium/high/xhigh/max, et `gpt-6-luna`, variantes none/low/medium/high/xhigh/max. Aucun `GPT-6.3.1 Sol` renvoyé par la recherche Sol : le brouillon conserve le nom natif existant plutôt que d'inventer un ID. Les entrées OpenCode Go et GLM existantes n'ont pas été remplacées.

Contrôle JSON via Context Mode : parsing réussi, seules les six clés de politique autorisées présentes, 29 couples uniques, providers cohérents. Ce contrôle n'est pas la validation native complète de `config`.

**Limite importante :** l'ordre du catalogue exprime la préférence dans la proposition, mais ne garantit pas une priorité stricte d'exécution. L'implémentation installée `selectModelForIntelligence` filtre par intelligence puis trie par coût ; un simple réordonnancement n'est pas un override. Les champs de préférence arbitraires sont interdits et aucune métrique n'a été falsifiée. Au seuil 50, Luna et GLM Flash restent sous le seuil initial ; Sol high atteint 50. Une restriction stricte exigerait une correction explicite de l'enveloppe autorisée, non appliquée ici puisqu'une préférence n'est pas une exclusion.

Aucun worker, classifier, test ou start lancé. Le hash de la précédente proposition ne valide pas ce brouillon modifié.

## Choix et prochaine action

Recommandation : représenter cette préférence sans supprimer les alternatives, conserver QA **proportionate**, puis faire présenter le brouillon réel par `/ensemble-mission config`. Les options light/reinforced restent possibles sur demande, sans supprimer les obligations ARTIST signalées dans [01-preparation.md](01-preparation.md). Leur arbitrage reste ouvert ; ce changement de modèles ne le tranche pas.

Décision demandée : relancer `config` pour examiner la politique modifiée ; corriger en chat si « GPT-6.3.1 Sol » désigne une autre connexion ou si une restriction stricte plutôt qu'une préférence est souhaitée. Le `start` reste exclusivement une commande utilisateur après présentation ; aucune reprise de pause déduite de cette correction.
