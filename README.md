# Sentinel

This is the repository for the Sentinel Kernel Module (SKM). Its role is to expose system-level metrics to userspace, by interfacing directly with the kernel

## Install

All the commands have to be run as `root`

* Install the Linux Kernel headers for your system
* Build the module with `make`
* Install the module with `make install`
* Enable auto-load with `echo "sentinel" >> /etc/modules-load.d/modules.conf`

## Adding new metrics 

There are a few places to edit to insert new metrics in the SKM.

* Create a new file entry in `/sys/kernel/sentinel` with the `sentinel_sysfs.c/init_attr_group()` function, by adding a new `case` to the `switch` statement. The new entry should look like:

```
case $X:
    sprintf(buf, "$NEW_METRIC_NAME");
    break;

```

where `$X` is the next number after the last `case` statemement, and `$NEW_METRIC_NAME` is the name of the file that should be created.

* Set the `sentinel_helper.c/NB_FILES_NOT_CPU` macro as `$X+1`.

* Edit the contents of `sentinel_helper.h/data_t` to add a new field for your metric.

* Edit the `sentinel_helper.c/populate_data()` or `sentinel_helper.c/get_process_data()` functions to fill the new fields with the corresponding data.

* Add a new `else if` statement to `sentinel_sysfs.c/sentinel_sysfs_show()` that formats and returns the value of your metric. The statement should look like:

```
else if (strcmp(attr->attr.name, "$NEW_METRIC_NAME") == 0) {
    return sprintf(buf, "$FORMAT_STRING\n", data.$FIELD_NAME);

```

where `$FORMAT_STRING` is the formatting to use, and `$FIELD_NAME` is the identifier of the metric field in the `data_t` structure.

* (Optional) If your metric should be writeable (to configure the SKM for instance), edit the `sentinel_sysfs.c/sentinel_sysfs_store()` to add a new `else if` statement that handles it. The statement should look like:

```
else if (strcmp(attr->attr.name, "$NEW_METRIC_NAME") == 0) {
    // Store or use the data
    return len;
```


## Possible improvements

* Only collect the value of the metric that was read, instead of gathering everything everytime.
* Make the code more modular to allow for easier metric adding (insetad of relying on the clunky `switch-case` and `else-if` statements everywhere)