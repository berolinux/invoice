#!/bin/bash
# Generate an invoice from data in Harvest (https://harvestapp.com/)
# SPDX-License-Identifier: GPL-2.0-or-later
# (C) 2022 Bernhard Rosenkränzer <bero@lindev.ch>
if [ -z "$HARVEST_ACCOUNT_ID" ] || [ -z "$HARVEST_TOKEN" ]; then
	echo "Set HARVEST_ACCOUNT_ID and HARVEST_TOKEN to a Personal Access Token"
	echo "You can get the Harvest Account ID and Token by going to"
	echo "https://id.getharvest.com/oauth2/access_tokens/new"
	echo "Set the token name to \"Auto-Invoicing\"."
	exit 1
fi
if [ -z "$HOURLY_RATE" ] && [ -z "$DAILY_RATE" ]; then
	echo "Set either HOURLY_RATE or DAILY_RATE to the amount you're paid."
	exit 1
fi

TODAY="$(date +%d)"
if [ "$TODAY" != "01" ]; then
	echo "This script is meant to be run by a cron job on the first day of a"
	echo "month, to collect invoicing data for the previous month. If you"
	echo "want to use it in a different context, edit it."
	exit 1
fi

ITEMS=""

YEAR="$(date --date=yesterday +%Y)"
MONTH="$(date --date=yesterday +%m)"
MONTHNAME="$(date --date=yesterday +%B)"
DAY="$(date --date=yesterday +%d)"

HOURS=0

while read r; do
	HOURS=$(echo $HOURS+$r |bc)
done < <(curl -L -i \
	-H "Harvest-Account-ID: $HARVEST_ACCOUNT_ID" \
	-H "Authorization: Bearer $HARVEST_TOKEN" \
	-H "User-Agent: Auto-Invoicing" \
	"https://api.harvestapp.com/api/v2/reports/time/clients?from=${YEAR}${MONTH}01&to=${YEAR}${MONTH}${DAY}" \
	2>/dev/null |tr ',' '\n' |grep '^"total_hours":' |cut -d: -f2)

if [ "$HOURS" != "0" ]; then
	if [ -n "$HOURLY_RATE" ]; then
		ITEMS="$ITEMS $HOURS \"Hours of work in $MONTHNAME\" $HOURLY_RATE"
	else
		DAYS="$(echo $HOURS/8.0 |bc)"
		ITEMS="$ITEMS $DAYS \"Days of work in $MONTHNAME\" $DAILY_RATE"
	fi
fi

while read r; do
	KEY=$(echo $r|cut -d: -f1 |sed -e 's,\",,g')
	VALUE=$(echo $r|cut -d: -f2-)
	if [ "$KEY" = "notes" ]; then
		DESCRIPTION="$VALUE"
	elif [ "$KEY" = "total_cost" ]; then
		ITEMS="$ITEMS 1 $DESCRIPTION $VALUE"
	fi
done < <(curl -L -i \
	-H "Harvest-Account-ID: $HARVEST_ACCOUNT_ID" \
	-H "Authorization: Bearer $HARVEST_TOKEN" \
	-H "User-Agent: Auto-Invoicing" \
	"https://api.harvestapp.com/api/v2/expenses?from=${YEAR}${MONTH}01&to=${YEAR}${MONTH}${DAY}" \
	2>/dev/null |tr ',' '\n' |grep -E '^"(notes|total_cost)":')

echo $ITEMS |xargs invoice
